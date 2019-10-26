#pragma once
#include <cstdlib/cstdint.h>

/**
 *                          Multiboot2 magic fields
 */
struct __attribute__((packed)) mboot_info_header {
    /**
     *                      Total size of the info block
     */
    uint32_t size;

    /**
     *                      Zero
     */
    uint32_t rsv;
};

static_assert(8 == sizeof(mboot_info_header), "mboot_info_header size is incorrect");

/**
 *                          Multiboot2 boot info tag types
 */
enum mb_info_tag_type : uint32_t {
    MB_INFO_MMAP = 6
};

/**
 *                          Multiboot2 boot information tag header
 */
struct __attribute__((packed)) mboot_tag_mmap {
    mb_info_tag_type type;
    uint32_t size;

    /**
     *                      Size of one map entry
     */
    uint32_t entry_size;

    /**
     *                      Currently only 0
     */
    uint32_t entry_version;

    // this struct is followed by a number of mboot_mmap_entry structs
};

static_assert(16 == sizeof(mboot_tag_mmap), "mboot_tag_mmap size is incorrect");

enum mb_mmap_type : uint32_t {
    MB_MMAP_AVAILABLE = 1,
    MB_MMAP_RECLAIMABLE = 3,
    MB_MMAP_NVS = 4,
    MB_MMAP_BADRAM = 5,
};

/**
 *                          Multiboot2 boot information tag header
 */
struct __attribute__((packed)) mboot_mmap_entry {
    /**
     *                      Physical address of the region
     */
    uint64_t addr;
    
    /**
     *                      Length of the region
     */
    uint64_t len;

    /**
     *                      Type of the region
     */
    mb_mmap_type type;

    /**
     *                      Zero
     */
    uint32_t rsv;
};

static_assert(24 == sizeof(mboot_mmap_entry), "mboot_mmap_entry size is incorrect");