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
#include <asm-generic/bitsperlong.h>
#include <linux/module.h>

MODULE_AUTHOR("KubaTaba1uga");
MODULE_DESCRIPTION("a simple LKM showing cpu informations in logs");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");

static int __init minimal_cpu_info_init(void)
{
	const char *cpu_family_name, *endianess;
	int word_length = -1;

	// In arch/x86/Kconfig you can find two config variables representing
	//   x86 architecture X86 and X86_64. These and other config variables
	//   can be accessed from within kernel src using CONFIG_* syntax.
#if defined(CONFIG_X86_32)
	cpu_family_name = "x86";
#elif defined(CONFIG_X86_64)
	cpu_family_name = "amd64";
	// In arch/arm/Kconfig we can find config variable representing ARM
	//    achitecture.
#elif defined(CONFIG_ARM)
	cpu_family_name = "arm";
	// In arch/arm64/Kconfig we can find config variable representing ARM64
	//    achitecture.
#elif defined(CONFIG_ARM64)
	cpu_family_name = "arm64";
#else
	cpu_family_name = "Undefined";
#endif

	// In include/asm-generic/bitsperlong.h (asm stands for assembler btw.)
	//   there are defined macro describing how many bits are used per long
	//   word.
#if defined(BITS_PER_LONG)	
	word_length = BITS_PER_LONG;
#else
	word_length = -1;
#endif

	// GCC or Clang sets __BYTE_ORDER__ macro but kernel also defines CONFIG_CPU_BIG_ENDIAN
	//   and CONFIG_CPU_LITTLE_ENDIAN in the same purpouse. You can find usage of both in
	//   various defconfigs. Tests showed that kernel configs are not always set so we are
	//   using __BYTE_ORDER__ as fallback mechanism.
#if defined(CONFIG_CPU_LITTLE_ENDIAN)
	endianess = "little endian";
#elif defined(CONFIG_CPU_BIG_ENDIAN)
	endianess = "big endian";
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	endianess = "little endian";
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	endianess = "big endian";
#else
	endianess = "Undefined";
#endif

	pr_info("Cpu architecture: %s\nLong Word length: %d\nEndianness: %s\n",
		cpu_family_name, word_length, endianess);
	return 0;		/* success */
}

static void __exit minimal_cpu_info_exit(void)
{
}

module_init(minimal_cpu_info_init);
module_exit(minimal_cpu_info_exit);
