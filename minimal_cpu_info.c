/*
 * minimal_cpu_info.c
 ****************************************************************
 * Brief Description:
 * A very simple module which prints to logs following informations about the cpu:
 *  - CPU architecture (x86, arm, amd64 etc.)
 *  - Long Word length
 *  - Endianness
 *
 * It wasn't clear for me at the beginning why I should use macros, shouldn't
 *    we fetch info somwhere from environment the module is being run on?
 *    When we are using macros we are doing somethin exactly opposite, we
 *    are hardcoding info in the binary. Let's examine the binary to clarify this:
              aarch64-linux-gnu-objdump -Dr minimal_cpu_info.ko
 *    The function above will print assembler for you, let's look onto <init_module>
 *    section: 
	      0000000000000008 <init_module>:
		 8:   d503201f        nop
		 c:   d503201f        nop
		10:   d503233f        paciasp
		14:   a9bf7bfd        stp     x29, x30, [sp, #-16]!
		18:   52800802        mov     w2, #0x40                       // #64
		1c:   910003fd        mov     x29, sp
		20:   90000003        adrp    x3, 0 <init_module-0x8>
				      20: R_AARCH64_ADR_PREL_PG_HI21  .rodata.str1.8
		24:   91000063        add     x3, x3, #0x0
				      24: R_AARCH64_ADD_ABS_LO12_NC   .rodata.str1.8
		28:   90000001        adrp    x1, 0 <init_module-0x8>
				      28: R_AARCH64_ADR_PREL_PG_HI21  .rodata.str1.8+0x10
		2c:   90000000        adrp    x0, 0 <init_module-0x8>
				      2c: R_AARCH64_ADR_PREL_PG_HI21  .rodata.str1.8+0x18
		30:   91000021        add     x1, x1, #0x0
				      30: R_AARCH64_ADD_ABS_LO12_NC   .rodata.str1.8+0x10
		34:   91000000        add     x0, x0, #0x0
				      34: R_AARCH64_ADD_ABS_LO12_NC   .rodata.str1.8+0x18
		38:   94000000        bl      0 <_printk>
				      38: R_AARCH64_CALL26    _printk
		3c:   52800000        mov     w0, #0x0                        // #0
		40:   a8c17bfd        ldp     x29, x30, [sp], #16
		44:   d50323bf        autiasp
		48:   d65f03c0        ret

 *   According to arm docs https://developer.arm.com/documentation/ddi0602/2024-12/Base-Instructions/ADRP--Form-PC-relative-address-to-4KB-page-?lang=en
 *   ADRP instruction is getting memory address from relative location and stores it in registry. I'm
 *   not an exper in assembly but i undesrtand it like this:
		There is some data in binary which is automatically load into memory, but program needs to
		fetch this automatically created address somehow. That's why we need ADRP instruction to get
		address of this automatically filled memory space and do not read from disk.
 *   So for example this:
		20:   90000003        adrp    x3, 0 <init_module-0x8>
				      20: R_AARCH64_ADR_PREL_PG_HI21  .rodata.str1.8
 *   Is setting x3 register to `.rodata.str1.8` section memory address.
 *   ADD instruction is just adding some offset to the memory address we obtain from ADRP, this is done
 *   to point to particullar data. So for example this:
		24:   91000063        add     x3, x3, #0x0
				      24: R_AARCH64_ADD_ABS_LO12_NC   .rodata.str1.8
 *   Loads address of memory and apply 0x0 offset. We can confirm that under 0x0 offset in .rodata.str1.8
 *   section is some valid string:
               aarch64-linux-gnu-objdump -s -j .rodata.str1.8 minimal_cpu_info.ko
 *   In my binary this is `little endian`:
	       minimal_cpu_info.ko:     file format elf64-littleaarch64

	       Contents of section .rodata.str1.8:
		0000 6c697474 6c652065 6e646961 6e000000  little endian...
		0010 61726d36 34000000 6d696e69 6d616c5f  arm64...minimal_
		0020 6370755f 696e666f 00000000 00000000  cpu_info........
		0030 01362573 3a257328 293a2043 70752061  .6%s:%s(): Cpu a
		0040 72636869 74656374 7572653a 2025730a  rchitecture: %s.
		0050 4c6f6e67 20576f72 64206c65 6e677468  Long Word length
		0060 3a202564 0a456e64 69616e6e 6573733a  : %d.Endianness:
		0070 2025730a 00                           %s..
 *   So code is setting fourth argument (x0 is first argument) of _printk via register x3 to `little endian`.
 *   This is matching what we could expect from our C code:
	       pr_info("Cpu architecture: %s\nLong Word length: %d\nEndianness: %s\n",
		       cpu_family_name, word_length, endianess); 
 *   Where `endianess` is fourth argument of the pr_info function. Ok so we confirmed that data is hardcoded
 *   in the binary (pretty big offtop i know xd) so finally let's understand why? Why hardcoding data in binary
 *   is okay if we want to create kernel module which should show some cpu info? Shouldn't we take this data
 *   dynamically, so our module would work on all possible architectures etc.?
 *   Maybe that's wierd but the anwser is no, we shouldn't. Look on stuff done in arch/arm64/kernel/cpuinfo.c
 *   which implements fetching data when you do:
                cat /proc/cpuinfo
 *   You can see that fetching data about cpu is pretty much architecture dependent thing. So in kernel space
 *   We do not have portible way to find out architecture of all porocessors beside writing bunch of assembly
 *   code which will fetch cpu data for each processor. However if we add one more card to the table the
 *   solution may seem easier to notice. Each kernel build is created per architetecture. So kernel build for
 *   x86 will not run on arm. This means that configuration specified during the kernel build needs to specify
 *   informations like cpu architecture. Each configuration set in .config file can be acessed from within
 *   kernel src code, making it vailable src of informations realting the machine that kernel will be run on. 
 */

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
	return 0;
}

static void __exit minimal_cpu_info_exit(void)
{
}

module_init(minimal_cpu_info_init);
module_exit(minimal_cpu_info_exit);
