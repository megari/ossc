#include <string.h>
#include "fat16_export.h"

/*
 * The beginning of the boot sector. Will be followed by the BPB.
 * Offsets 0x003 to 0x01c, inclusive.
 * The BPB spans offsets 0x00b to 0x01c, inclusive.
 */
static const alt_u8 bootsec_beg_bpb_16[24] = {
    0x4d, 0x53, 0x57, 0x49, 0x4e, 0x34, 0x2e, 0x31,
    0x00, 0x02, 0x04, 0x80, 0x00, 0x02, 0x00, 0x08,
    0x00, 0x80, 0xf8, 0x20, 0x00, 0x3f, 0x00, 0xff,
};

/*
 * The rest of the boot sector before the boot code and terminator.
 * Offsets 0x024 to 0x03d, inclusive.
 */
static const alt_u8 bootsec_after_bpb_16[26] = {
    0x80, 0x00, 0x29, 0xf4, 0xcf, 0xc6, 0x04, 0x4f, 0x53, 0x53, 0x43, 0x50,
    0x52, 0x4f, 0x46, 0x49, 0x4c, 0x53, 0x46, 0x41, 0x54, 0x31, 0x36, 0x20,
    0x20, 0x20,
};

/*
 * After this, we have the boot code (448 bytes) and sector terminator
 * (2 bytes). These will be generated.
 */

/* Generates a FAT16 boot sector.
 * buf must be at least FAT16_SECTOR_SIZE bytes long,
 * and is assumed to be pre-zeroed.
 */
void generate_boot_sector_16(alt_u8 *const buf) {
    /* Initial FAT16 boot sector contents. */
    memcpy(buf + 3, bootsec_beg_bpb_16, 24);

    /*
     * Then the rest of the boot sector.
     * The boot code is just 448 bytes of 0xf4.
     */
    memcpy(buf + 36, bootsec_after_bpb_16, 26);

    /* Leave the boot code zeroed out. Ugly, but should decrease code size. */

    /* RISC-V is little-endian, so do a 16-bit write instead. */
    *((alt_u16*)(buf + 510)) = 0xaa55U;
}

/* The fixed 'preamble' of a FAT on a FAT16 volume. */
static const alt_u32 fat16_preamble = 0xfffffff8U;

/*
 * Generate a FAT.
 * The buffer is assumed to be zeroed out and have a size of at least
 * FAT16_SECTOR_SIZE bytes.
 * The number of clusters already written is given as an argument.
 * The function returns the total number of clusters written so far.
 *
 * The intention is to be able to generate and write the FAT in chunks
 * that do not exhaust all the remaining RAM.
 */
alt_u16 generate_fat16(void *const buf, const alt_u16 written) {
	alt_u16 cur_ofs = 0;
    const alt_u16 start_cluster = 3U + written;
    alt_u16 *const fat = buf;

    /*
     * The total number of FAT entries to write consists of:
     * 1. The FAT "preamble" (2 entries),
     * 2. The cluster chain of the file (512 entries).
     *
     * The latter needs to contain the chain terminator.
     */
    const alt_u16 clusters_remaining = PROF_16_CLUSTER_COUNT - written;
    const alt_u16 preamble_compensation = written ? 0 : 2U;
    const alt_u16 clusters_to_write =
        ((clusters_remaining > FAT16_ENTRIES_PER_SECTOR)
            ? FAT16_ENTRIES_PER_SECTOR
            : clusters_remaining) - preamble_compensation;
    const alt_u16 end_cluster = start_cluster + clusters_to_write;
    static const alt_u16 last_fat_cluster = PROF_16_CLUSTER_COUNT + 2U;

    if (!written) {
        *((alt_u32*)fat) = fat16_preamble;
        cur_ofs += sizeof(fat16_preamble)/sizeof(alt_u16);
    }

    for (alt_u16 cluster = start_cluster; cluster < end_cluster; ++cluster) {
        alt_u16 *const cur_entry = fat + cur_ofs;
        /* FAT16 entries are 16-bit little-endian. */
        if (cluster == last_fat_cluster) {
            /* At the last cluster, write the chain terminator. */
            *cur_entry = 0xffffU;
        }
        else {
            *cur_entry = cluster;
        }
        ++cur_ofs;
    }

    return end_cluster - 3U;
}

const alt_u8 prof_dirent_16[PROF_DIRENT_16_SIZE] = {
    0x4f, 0x53, 0x53, 0x43, 0x50, 0x52, 0x4f, 0x46, 0x42, 0x49, 0x4e, 0x20,
    0x00, 0x8e, 0x04, 0xb5, 0x6f, 0x51, 0x6f, 0x51, 0x00, 0x00, 0x17, 0x89,
    0x6f, 0x51, 0x02, 0x00, 0x00, 0x00, 0x10, 0x00,
};
