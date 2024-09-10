#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/sched.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/memory.h>
#include <linux/mm.h>
#include <linux/atomic.h>

#include <paging.h>

static  atomic_t num_pages_allocated = ATOMIC_INIT(0);
static  atomic_t num_pages_freed = ATOMIC_INIT(0);

struct tracker {
     atomic_t ref_counter; // Reference ref_counter TO THIS VMA
        // WE can have multiple processes referencing the same VMA
    struct list_head pages; // Linked list where every node is a pointer to a page
};

struct page_node {
    struct page * p;
    struct list_head list;
};

static int
do_fault(struct vm_area_struct * vma,
         unsigned long           fault_address)
{
    printk(KERN_INFO "paging_vma_fault() invoked: took a page fault at VA 0x%lx\n", fault_address);
    return VM_FAULT_SIGBUS;
}


static vm_fault_t
paging_vma_fault(struct vm_fault * vmf)
{
    struct page * phys_addr;
    struct vm_area_struct * vma;
    struct tracker * vma_tracker;
    struct page_node * new_page_node;
    unsigned long page_frame_num;
    unsigned long fault_address;
    unsigned long virtual_address;

    int err;

    vma = vmf->vma;
    fault_address = (unsigned long)vmf->address;

    // struct page * phys_addr = alloc_pages(GFP_KERNEL, my_get_order(1));
    phys_addr = alloc_page(GFP_KERNEL);

    if (!phys_addr) {
        printk( " alloc_page failed !\n");
        return VM_FAULT_OOM;
    }

    vma_tracker = vma->vm_private_data;
    new_page_node = kmalloc(sizeof(struct page_node), GFP_KERNEL);

    new_page_node->p = phys_addr;
    list_add(&(new_page_node->list), &(vma_tracker->pages));

    atomic_inc(&num_pages_allocated);

    page_frame_num = page_to_pfn(phys_addr);
    if (!page_frame_num) {
        return VM_FAULT_SIGBUS;
    }

    virtual_address = PAGE_ALIGN(fault_address);
    if (!virtual_address) {
        return VM_FAULT_SIGBUS;
    }

    // length is just the size of a page because this is a page fault :)))
    err = remap_pfn_range(vma, virtual_address, page_frame_num, PAGE_SIZE, vma->vm_page_prot);

    if (err < 0 ) { // TODO: Wrote this error code wrong the first time
        
        return VM_FAULT_NOPAGE;
    } // else handled by do_fault at the moment

    return do_fault(vma, fault_address);
}

static void
paging_vma_open(struct vm_area_struct * vma)
{
    // Opens a VMA

    struct tracker * vma_tracker = vma->vm_private_data;

    atomic_inc(&vma_tracker->ref_counter);

    // Run compare exchange over ref_counter here
    // Return an error if unexpected  

    printk(KERN_INFO "paging_vma_open() invoked\n");
}

static void
paging_vma_close(struct vm_area_struct * vma)
{
    // Closes a VMA

    struct tracker * vma_tracker = vma->vm_private_data;

    // Potential race condition between close and open
    // Here we decrement and check if it is 0 in one atomic operation
    if (atomic_dec_and_test(&vma_tracker->ref_counter)) {

        struct page_node * p_n;
        // List is the name of the type in page_node that represents a pointer to the next page_node
        list_for_each_entry(p_n, &(vma_tracker->pages), list) {
            __free_page(p_n->p);
            //printk(" freed a page \n");
            atomic_inc(&num_pages_freed);
        }

        // TODO: Free wrapper structs off of heap

    }


    printk(KERN_INFO "paging_vma_close() invoked\n");
}

static struct vm_operations_struct
paging_vma_ops = 
{
    .fault = paging_vma_fault,
    .open  = paging_vma_open,
    .close = paging_vma_close
};

/* vma is the new virtual address segment for the process */
static int
paging_mmap(struct file           * filp,
            struct vm_area_struct * vma)
{

    struct tracker * vma_tracker;

    /* prevent Linux from mucking with our VMA (expanding it, merging it 
     * with other VMAs, etc.)
     */
    vma->vm_flags |= VM_IO | VM_DONTCOPY | VM_DONTEXPAND | VM_NORESERVE
              | VM_DONTDUMP | VM_PFNMAP;

    /* setup the vma->vm_ops, so we can catch page faults */
    vma->vm_ops = &paging_vma_ops;


    // Here a process requests to create a virtual memory area with mmap
    // all mappings of pages into the VMA are tracked by a tracker
    // so we make one tracker per VMA


    vma_tracker = kmalloc(sizeof(struct tracker), GFP_KERNEL);

    atomic_set(&vma_tracker->ref_counter, 1);

    INIT_LIST_HEAD(&vma_tracker->pages);

    
    vma->vm_private_data = vma_tracker;

    // Since mmap is being called, we have at least one processes (the caller)
    // referencing this VMA

    printk(KERN_INFO "paging_mmap() invoked: new VMA for pid %d from VA 0x%lx to 0x%lx\n",
        current->pid, vma->vm_start, vma->vm_end);

    return 0;
}

static struct file_operations
dev_ops =
{
    .mmap = paging_mmap,
};

static struct miscdevice
dev_handle =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = PAGING_MODULE_NAME,
    .fops = &dev_ops,
};
/*** END device I/O **/

/*** Kernel module initialization and teardown ***/
static int
kmod_paging_init(void)
{
    int status;

    /* Create a character device to communicate with user-space via file I/O operations */
    status = misc_register(&dev_handle);
    if (status != 0) {
        printk(KERN_ERR "Failed to register misc. device for module\n");
        return status;
    }

    printk(KERN_INFO "Loaded kmod_paging module\n");

    return 0;
}

static void
kmod_paging_exit(void)
{
    /* Deregister our device file */
    misc_deregister(&dev_handle);

    printk(KERN_INFO "Unloaded kmod_paging module\n");
    
    printk("num_pages_allocated: %d , num_pages_freed: %d \n", atomic_read(&num_pages_allocated), atomic_read(&num_pages_freed));

}

module_init(kmod_paging_init);
module_exit(kmod_paging_exit);

/* Misc module info */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Who are you?");
MODULE_DESCRIPTION("Please describe this module.");
