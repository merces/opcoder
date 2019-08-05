/*
// Architecture type
typedef enum ks_arch {
    KS_ARCH_ARM = 1,    // ARM architecture (including Thumb, Thumb-2)
    KS_ARCH_ARM64,      // ARM-64, also called AArch64
    KS_ARCH_MIPS,       // Mips architecture
    KS_ARCH_X86,        // X86 architecture (including x86 & x86-64)
    KS_ARCH_PPC,        // PowerPC architecture (currently unsupported)
    KS_ARCH_SPARC,      // Sparc architecture
    KS_ARCH_SYSTEMZ,    // SystemZ architecture (S390X)
    KS_ARCH_HEXAGON,    // Hexagon architecture
    KS_ARCH_MAX,
} ks_arch;

// Mode type
typedef enum ks_mode {
    KS_MODE_LITTLE_ENDIAN = 0,    // little-endian mode (default mode)
    KS_MODE_BIG_ENDIAN = 1 << 30, // big-endian mode
    // arm / arm64
    KS_MODE_ARM = 1 << 0,              // ARM mode
    KS_MODE_THUMB = 1 << 4,       // THUMB mode (including Thumb-2)
    KS_MODE_V8 = 1 << 6,          // ARMv8 A32 encodings for ARM
    // mips
    KS_MODE_MICRO = 1 << 4,       // MicroMips mode
    KS_MODE_MIPS3 = 1 << 5,       // Mips III ISA
    KS_MODE_MIPS32R6 = 1 << 6,    // Mips32r6 ISA
    KS_MODE_MIPS32 = 1 << 2,      // Mips32 ISA
    KS_MODE_MIPS64 = 1 << 3,      // Mips64 ISA
    // x86 / x64
    KS_MODE_16 = 1 << 1,          // 16-bit mode
    KS_MODE_32 = 1 << 2,          // 32-bit mode
    KS_MODE_64 = 1 << 3,          // 64-bit mode
    // ppc 
    KS_MODE_PPC32 = 1 << 2,       // 32-bit mode
    KS_MODE_PPC64 = 1 << 3,       // 64-bit mode
    KS_MODE_QPX = 1 << 4,         // Quad Processing eXtensions mode
    // sparc
    KS_MODE_SPARC32 = 1 << 2,     // 32-bit mode
    KS_MODE_SPARC64 = 1 << 3,     // 64-bit mode
    KS_MODE_V9 = 1 << 4,          // SparcV9 mode
} ks_mode;
 */

#include <stdio.h>
#include <keystone/keystone.h>
#include <getopt.h>
#include <string.h>

// separate assembly instructions by ; or \n
#define CODE "nop"
#define PROGRAM "opcoder"

#define OPCODER_OUTPUT_C 1

typedef struct _options {
    unsigned arch;
    unsigned mode;
} options;

options opt;

static void usage(void)
{
	printf("Usage: %s OPTIONS <assembly code>\n"
		"Opcode compiler\n"
		"\nExample: %s -aarm 'nop'\n"
		"\nOptions:\n"
		" -a, --arch                             compile for the following architecture\n"
		" --help                                 show this help and exit\n",
		PROGRAM, PROGRAM);
}

static void parse_options(int argc, char *argv[])
{
	/* Parameters for getopt_long() function */
	static const char short_options[] = "ha:";

	static const struct option long_options[] = {
		{ "arch",            required_argument,  NULL, 'a' },
        { "help",            no_argument,        NULL, 'h'  },
		{ NULL,              0,                  NULL,  0  }
	};

	int c, ind;

	while ((c = getopt_long(argc, argv, short_options, long_options, &ind)))
	{
		if (c < 0)
			break;

		switch (c)
		{
			case 'h':		// --help option
				usage();
				exit(0);
			case 'a':
			{
				if (strncmp(optarg, "arm", 3) == 0) {
                    opt.arch = KS_ARCH_ARM;
                    opt.mode = KS_MODE_ARM;
                } else if (strncmp(optarg, "x86", 3) == 0) {
                    opt.arch = KS_ARCH_X86;
                    opt.mode = KS_MODE_32;
                } else if (strncmp(optarg, "x64", 3) == 0) {
                    opt.arch = KS_ARCH_X86;
                    opt.mode = KS_MODE_64;
                } else if (strncmp(optarg, "arm64", 5) == 0) {
                    opt.arch = KS_ARCH_X86;
                    opt.mode = KS_MODE_64;
                } else if (strncmp(optarg, "mips", 4) == 0) {
                    opt.arch = KS_ARCH_MIPS;
                } else if (strncmp(optarg, "ppc", 3) == 0) {
                    opt.arch = KS_ARCH_PPC;
                } else {
                    usage();
                    exit(1);
                }
				break;
			}
			default:
				fprintf(stderr, "%s: try '-h' or '--help' for more information\n", PROGRAM);
				exit(1);
		}
	}
}

int main(int argc, char *argv[])
{
    ks_engine *ks;
    ks_err err;
    size_t count;
    unsigned char *encode;
    size_t size;
    char *code = argv[argc - 1];

    if (argc < 2) {
        usage();
        exit(1);
    }

    // Default arch/mode
    opt.arch = KS_ARCH_X86;
    opt.mode = KS_MODE_32;

    parse_options(argc, argv);

    err = ks_open(opt.arch, opt.mode, &ks);

    if (err != KS_ERR_OK) {
        printf("ERROR: failed on ks_open(), quit\n");
        return 1;
    }

    if (ks_asm(ks, code, 0, &encode, &size, &count) != KS_ERR_OK) {
        printf("ERROR: ks_asm() failed & count = %lu, error = %u\n",
        count, ks_errno(ks));
        return 1;
    }

    size_t i;

    // int
    printf("0x");
    for (i=0; i < size; i++)
        printf("%02x", encode[i]);
    
    // C shellcode
    printf("\n");
    for (i=0; i < size; i++)
        printf("\\x%02x", encode[i]);
    
    // Hex bytes
    printf("\n");
    for (i=0; i < size; i++)
        printf("%02x ", encode[i]);

    printf("\nCompiled: %lu bytes, statements: %lu\n", size, count);

    ks_free(encode);
    ks_close(ks);

    return 0;
}
