#pragma once
#include <cstdlib/cstdint.h>

namespace mm {
    /**
     *                      Anatomy of a virtual address.
     */
    struct __attribute__((packed)) virtual_address {
        /**
         *                  Zero
         */
        unsigned rsv0 : 12;
        
        /**
         *                  Index into a page table
         */
        unsigned idx_pt : 9;
        
        /**
         *                  Index into a page directory
         */
        unsigned idx_pd : 9;
        
        /**
         *                  Index into a page directory pointer table
         */
        unsigned idx_pdpt : 9;
        
        /**
         *                  Index into a page map level 4 table
         */
        unsigned idx_pml4 : 9;
        
        /**
         *                  Unused
         */
        unsigned rsv1 : 16;
    };

    static_assert(8 == sizeof(virtual_address), "virtual_address size is incorrect");

    /**
     *                      Structure of a level 2-4 page map entry.
     *                      For our purposes these 3 levels have the same structure
     */
    struct __attribute__((packed)) page_map_entry {
        /**
         * true             Entry is valid
         */
        bool present : 1;
        /**
         * true             Writes are allowed to the region referenced
         */
        bool rw : 1;

        /**
         * true             User mode accesses are allowed to the region referenced
         */
        bool user : 1;

        /**
         *                  Page-level write through
         */
        bool pwt : 1;

        /**
         *                  Page-level cache disable
         */
        bool pcd : 1;

        /**
         *  true            Entry has been used in a translation
         */
        bool accessed : 1;

        /**
         *                  Ignored
         */
        unsigned ign0 : 1;

        /**
         *                  Zero
         */
        unsigned rsv0 : 1;

        /**
         *                  Ignored
         */
        unsigned ign1 : 4;

        /**
         *                  4KB-aligned pdpt address 
         */
        unsigned addr : 40;

        /**
         *                  Ignored
         */
        unsigned ign2 : 12;
    };

    static_assert(8 == sizeof(page_map_entry), "page_map_entry size is incorrect");

    /**
     *                      Page map level 2-4 structure
     */
    struct __attribute__((packed)) page_map {
        page_map_entry entries[512];
    };

    static_assert(4096 == sizeof(page_map), "page_map size is incorrect");

    /**
     *                      Structure of a page map level 1 entry
     */
    struct __attribute__((packed)) page_table_entry {
        /**
         * true             Entry is valid
         */
        bool present : 1;
        /**
         * true             Writes are allowed to the 1GB region referenced
         */
        bool rw : 1;

        /**
         * true             User mode accesses are allowed to the 1GB region referenced
         */
        bool user : 1;

        /**
         *                  Page-level write through
         */
        bool pwt : 1;

        /**
         *                  Page-level cache disable
         */
        bool pcd : 1;

        /**
         *  true            Page has been accessed
         */
        bool accessed : 1;

        /**
         * true             This page has been written to
         */
        bool dirty : 1;

        /**
         *                  memory type used to access the 4KiB page
         */
        bool pat : 1;

        /**
         *                  Ignored
         */
        unsigned ign1 : 4;

        /**
         *                  physical address of 4KB page
         */
        unsigned addr : 40;

        /**
         *                  Ignored
         */
        unsigned ign2 : 12;
    };

    static_assert(8 == sizeof(page_table_entry), "page_table_entry size is incorrect");

    /**
     *                      Page map level 1
     */
    struct __attribute__((packed)) page_table {
        page_table_entry entries[512];
    };

    static_assert(4096 == sizeof(pdpt), "page_table size is incorrect");

    /**
     *                      A representation of one 4KiB page in memory.
     *                      These will be placed after the kernel binary once memory is detected.
     *                      Some of these pages will be the beginning of a block.
     */
    struct page {
        /**
         *                  The virtual address assigned to this page
         */
        virtual_address va;

        /**
         *                  The virtual address space that the page belongs to (pml4)
         *                  (both this and above might not be necessary, put these in virtual memory manager instead)
         */
        page_map* virtual_space;

        /**
         *                  The block's order.
         *                  This may range from [0, max_order] and 2^order
         *                  pages following this one are are part of the same block
         */
        unsigned char order;
    };

    // TODO also need a struct zone to manage a set of the above page struct for each contiguous free zone of memory
}