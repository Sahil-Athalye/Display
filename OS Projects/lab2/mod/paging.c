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
#include <linux/list.h>
#include <linux/atomic.h>
#include <paging.h>

// Enable pre-paging by passing demand_paging=0
static unsigned int demand_paging = 1;
module_param(demand_paging, uint, 0644);

// Module globals for tracking page allocations (atomic)
static  atomic_t num_pages_allocated = ATOMIC_INIT(0);
static  atomic_t num_pages_freed = ATOMIC_INIT(0);

struct tracker {
     atomic_t ref_counter; // Number of processes holding a reference to this VMA.
    struct list_head pages_list; // Kernel list that tracks all pages allocated in our VMA.
};

// A wrapper struct for our kernel list implementation
// each node in this list points to a page
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
    unsigned long page_frame_num;
    unsigned long virtual_address;
    unsigned long fault_address;
    int err;
    struct tracker * vma_tracker;
    struct page_node * new_page_node;

    vma = vmf->vma;
    fault_address = (unsigned long)vmf->address;

    phys_addr = alloc_page(GFP_KERNEL);

    if (!phys_addr) {
        printk( " alloc_page failed !\n");
        return VM_FAULT_OOM;
    }

    page_frame_num = page_to_pfn(phys_addr);
    if (!page_frame_num) {
         printk(" page_to_pfn failed !\n");
        return do_fault(vma, fault_address);
    }

    virtual_address = PAGE_ALIGN(fault_address);
    if (!virtual_address) {
         printk(" PAGE_ALIGN failed !\n");
        return do_fault(vma, fault_address);
    }

    err = remap_pfn_range(vma, virtual_address, page_frame_num, PAGE_SIZE, vma->vm_page_prot);
    if (err < 0) {
        printk(" remap_pfn_range failed !\n");
        return do_fault(vma, fault_address);
    }

    // Update our VMA tracker
    vma_tracker = vma->vm_private_data;
    new_page_node = kmalloc(sizeof(struct page_node), GFP_KERNEL);
    new_page_node->p = phys_addr;
    list_add(&(new_page_node->list), &(vma_tracker->pages_list));
    atomic_inc(&num_pages_allocated);

    return VM_FAULT_NOPAGE;
}

// Opens a VMA
static void
paging_vma_open(struct vm_area_struct * vma)
{
    struct tracker * vma_tracker = vma->vm_private_data;
    atomic_inc(&vma_tracker->ref_counter);
    printk(KERN_INFO "paging_vma_open() invoked\n");
}


// Closes a VMA
static void
paging_vma_close(struct vm_area_struct * vma)
{
    struct tracker * vma_tracker = vma->vm_private_data;

    // Due to a potential race condition between open() and close()
    // we use dec_and_test here.
    if (atomic_dec_and_test(&vma_tracker->ref_counter)) {

        struct page_node * node_current;
        struct page_node * node_next;

        // Iterate over every node in our tracker pages_list
        list_for_each_entry_safe(node_current, node_next, &(vma_tracker->pages_list), list) {
            
            // Free the physical page our node points to
            __free_page(node_current->p);
            atomic_inc(&num_pages_freed);

            // Remove our page_node from the linked list
            list_del(&node_current->list);

            // Since all page_nodes were allocated on the heap, we need to free each node
            kfree(node_current);
        }
    }

    // Finally, de-allocate our tracker struct
    kfree(vma->vm_private_data);

    printk(KERN_INFO "paging_vma_close() invoked\n");
}

static struct vm_operations_struct
paging_vma_ops = 
{
    .fault = paging_vma_fault,
    .open  = paging_vma_open,
    .close = paging_vma_close
};

static vm_fault_t pre_page_alloc(struct vm_area_struct * vma) {

        struct page * phys_addr;
        struct tracker * vma_tracker;
        struct page_node * new_page_node;
        unsigned long page_frame_num;
        unsigned long virtual_address;
        int err;

        // Iterate over every virtual page address from start to finish of our VMA
        for (virtual_address = vma->vm_start ; virtual_address < vma->vm_end; virtual_address += PAGE_SIZE) {
            
            // Allocate a physical page, and map it to this virtual page address

            phys_addr = alloc_page(GFP_KERNEL);
            if (!phys_addr) {
                printk( " alloc_page failed !\n");
                return VM_FAULT_OOM;
            }

            page_frame_num = page_to_pfn(phys_addr);
            if (!page_frame_num) {
                printk(" page_to_pfn failed !\n");
                return VM_FAULT_SIGBUS;
            }

            err = remap_pfn_range(vma, virtual_address, page_frame_num, PAGE_SIZE, vma->vm_page_prot);
            if (err < 0) {
                printk(" remap_pfn_range failed !\n");
                return VM_FAULT_SIGBUS;
            }

            // Update our VMA tracker
            vma_tracker = vma->vm_private_data;
            new_page_node = kmalloc(sizeof(struct page_node), GFP_KERNEL);
            new_page_node->p = phys_addr;
            list_add(&(new_page_node->list), &(vma_tracker->pages_list));
            atomic_inc(&num_pages_allocated);
        }

        return VM_FAULT_NOPAGE;

}

/* vma is the new virtual address segment for the process */
static int
paging_mmap(struct file           * filp,
            struct vm_area_struct * vma)
{
    
    // Create a tracker for the requested VMA
    struct tracker * vma_tracker;
    vma_tracker = kmalloc(sizeof(struct tracker), GFP_KERNEL);
    atomic_set(&vma_tracker->ref_counter, 1); // The process that made this mmap call, is the first reference to this VMA
    INIT_LIST_HEAD(&vma_tracker->pages_list);
    vma->vm_private_data = vma_tracker;

    /* prevent Linux from mucking with our VMA (expanding it, merging it 
    * with other VMAs, etc.)
    */
    vma->vm_flags |= VM_IO | VM_DONTCOPY | VM_DONTEXPAND | VM_NORESERVE
            | VM_DONTDUMP | VM_PFNMAP;

    /* setup the vma->vm_ops, so we can catch page faults */
    vma->vm_ops = &paging_vma_ops;

    printk(KERN_INFO "paging_mmap() invoked: new VMA for pid %d from VA 0x%lx to 0x%lx\n",
        current->pid, vma->vm_start, vma->vm_end);

    if (!demand_paging) { // Use pre-paging when demand_paging is turned off
        vm_fault_t ret = pre_page_alloc(vma);
        if (ret == VM_FAULT_OOM) {
            return -ENOMEM;
        }
        else if (ret == VM_FAULT_SIGBUS) {
            return -EFAULT;
        }
    }

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

    if (demand_paging) {
        printk(KERN_INFO "Loaded kmod_paging module with demand paging\n");
    }
    else { 
        printk(KERN_INFO "Loaded kmod_paging module with pre-paging\n");
    }

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
