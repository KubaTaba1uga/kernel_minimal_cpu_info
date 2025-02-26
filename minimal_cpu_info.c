/*
 * minimal_cpu_info.c
 ****************************************************************
 * Brief Description:
 * A very simple module which prints to logs following informations about the cpu:
 *  - CPU architecture (x86, arm, amd64 etc.)
 *  - Long Word length
 *  - Endianness
 */

#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>

MODULE_AUTHOR("KubaTaba1uga");
MODULE_DESCRIPTION("a simple LKM showing cpu informations in logs");
MODULE_LICENSE("Dual MIT/GPL");	
MODULE_VERSION("0.1");

static int __init minimal_cpu_info_init(void)
{const char * cpu_family_name = "Unkown", *endianess = "Unkown";
  int word_length = -1;

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

       // In include/asm-generic/bitsperlong.h (asm stands for assembler btw.)
       //   there are defined macro describing how many bits are used per long
       //   word.
       word_length = BITS_PER_LONG;

       // GCC or Clang sets __BYTE_ORDER__ macro but kernel also defines CONFIG_CPU_BIG_ENDIAN
       //   and CONFIG_CPU_LITTLE_ENDIAN in the same purpouse. You can find usage of both in
       //   various defconfigs.
       #ifdef CONFIG_CPU_LITTLE_ENDIAN
       endianess="little endian";
       #endif       
       #ifdef CONFIG_CPU_BIG_ENDIAN
       endianess="big endian";
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
