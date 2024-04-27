#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

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
	{ 0xa53b631e, "usb_submit_urb" },
	{ 0x122c3a7e, "_printk" },
	{ 0x34db050b, "_raw_spin_lock_irqsave" },
	{ 0xd35cce70, "_raw_spin_unlock_irqrestore" },
	{ 0x4899aedf, "_dev_err" },
	{ 0x26fe556, "_dev_warn" },
	{ 0x38b03f0f, "usb_deregister" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0xf87e362d, "input_event" },
	{ 0xf812cff6, "memscan" },
	{ 0x93581336, "_dev_info" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xab69f068, "kmalloc_caches" },
	{ 0x5c6fcbb3, "kmalloc_trace" },
	{ 0x6b00a8f8, "input_allocate_device" },
	{ 0xfa5417e, "usb_alloc_urb" },
	{ 0xd7061dff, "usb_alloc_coherent" },
	{ 0xa916b694, "strnlen" },
	{ 0xdd64e639, "strscpy" },
	{ 0x754d539c, "strlen" },
	{ 0xf3b9696, "input_free_device" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x8fb54838, "input_register_device" },
	{ 0xa9a9cf97, "device_set_wakeup_enable" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xa984d4db, "usb_register_driver" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xec0ec2ad, "usb_kill_urb" },
	{ 0x4ba67399, "usb_free_urb" },
	{ 0xb45b9163, "usb_free_coherent" },
	{ 0x37a0cba, "kfree" },
	{ 0x7c3444f4, "input_unregister_device" },
	{ 0x6ab589bc, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic03isc01ip01in*");

MODULE_INFO(srcversion, "5B60D18A4D08F5F615A6A26");
