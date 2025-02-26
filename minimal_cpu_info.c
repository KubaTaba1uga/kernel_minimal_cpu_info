/*
 * minimal_cpu_info.c
 ****************************************************************
 * Brief Description:
 * A very simple module which prints to logs following informations about the cpu:
 *  - CPU architecture (x86, arm, amd64 etc.)
 *  - Word length
 *  - Endianess
 */

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>

MODULE_AUTHOR("KubaTaba1uga");
MODULE_DESCRIPTION("a simple LKM showing cpu informations in logs");
MODULE_LICENSE("Dual MIT/GPL");	
MODULE_VERSION("0.1");



static int __init minimal_cpu_info_init(void)
{const char * cpu_family_name, word_length, endianess;

       // In arch/x86/Kconfig you can find two config variables representing
       //   x86 architecture X86 and X86_64. These and other config variables
       //   can be accessed from within kernel src using CONFIG_* syntax.
       #ifdef CONFIG_X86
       cpu_family_name = "x86";
       #endif
       #ifdef CONFIG_X86_64
       cpu_family_name = "amd64";
       #endif

       // In arch/arm/Kconfig we can find config variable representing ARM
       //    achitecture.
       #ifdef CONFIG_ARM
       cpu_family_name = "arm";
       #endif

       // In arch/arm64/Kconfig we can find config variable representing ARM64
       //    achitecture.
       #ifdef CONFIG_ARM64
       cpu_family_name = "arm64";
       #endif

  
	pr_info("inserted\n");
	return 0;		/* success */
}

static void __exit minimal_cpu_info_exit(void)
{
	pr_info("removed\n");
}



module_init(minimal_cpu_info_init);
module_exit(minimal_cpu_info_exit);
