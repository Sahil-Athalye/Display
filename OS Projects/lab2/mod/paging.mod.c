#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x70f1bb7f, "module_layout" },
	{ 0xf21ef7f4, "param_ops_uint" },
	{ 0xb971c960, "misc_deregister" },
	{ 0xe6f4897e, "misc_register" },
	{ 0x37a0cba, "kfree" },
	{ 0x91c92802, "__free_pages" },
	{ 0xc5850110, "printk" },
	{ 0xa6065a48, "kmem_cache_alloc_trace" },
	{ 0xa4edf4e7, "kmalloc_caches" },
	{ 0x8e99e8a7, "remap_pfn_range" },
	{ 0x987c11c7, "__pv_phys_pfn_offset" },
	{ 0x9c133398, "mem_map" },
	{ 0x2330343, "__alloc_pages_nodemask" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "ADD1F1DAAB29410A6D43034");
