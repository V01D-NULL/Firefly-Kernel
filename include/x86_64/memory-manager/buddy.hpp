#ifndef __BUDDY_HPP__
#define __BUDDY_HPP__

#include <array.h>
#include <cstdlib/cmath.h>
#include <cstdlib/cstdint.h>

#include "buddy_heap.hpp"

namespace firefly::kernel::mm::buddy {

constexpr static inline size_t SMALLEST_CHUNK = 4096UL;                // 1 page
constexpr static inline size_t LARGEST_CHUNK = SMALLEST_CHUNK * 16UL;  // 16 pages

// casually casting a double to size_t because we _know_ that a power of 2 divided by a power of 2 that's larger
// will return an integer.
constexpr static inline uint8_t MAXIMUM_ORDER = static_cast<uint8_t>(constexpr_log2(LARGEST_CHUNK / SMALLEST_CHUNK)) - 1;

struct Chunk;
class BuddyAllocator;

// Do we want to store amount of memory that has been taken for the so-called zero nodes?
// probably not cause that memory doesn't have to be located next to eachother
struct BuddyNode {
    void* physical_addr;  // this is always the same as split_one's physical address...

    uint8_t order : 5;  // ORDER HAS TO BE SMALLER THAN 0b11111 (32)
    uint8_t _is_taken : 1;
    uint8_t _is_split : 1;
    uint8_t _is_right : 1;

    size_t pointed_size() const noexcept;

    bool is_split() const noexcept;
    bool is_taken() const noexcept;
    bool is_right() const noexcept;

    BuddyNode* get_left_child();
    BuddyNode* get_right_child();
    BuddyNode* get_parent();
    BuddyNode* get_matching_buddy();

    // if the taken_until pointer is null and this node isn't split
    // that means that this node hasn't yet been allocated from
    bool is_free() const noexcept;

    Chunk* to_chunk(BuddyAllocator* buddy) const noexcept;

    void split(BuddyAllocator* buddy) noexcept;
    void merge_children(BuddyAllocator* buddy, Chunk* matching_chunk = nullptr) noexcept;
};

struct Chunk {
    BuddyNode* root = nullptr;
    size_t heap_index;
    uint8_t can_be_allocated : 1 = 1;
    firefly::std::array<int8_t, MAXIMUM_ORDER + 1> free_values = {};


    void add_order(size_t order, int count);
    bool can_allocate(uint8_t order) const noexcept;
    BuddyNode* get_free_buddy(BuddyAllocator* buddy, uint8_t order) noexcept;
    void fix_heap(BuddyAllocator* buddy);
};

class BuddyAllocator {
    friend BuddyTreeHeap;
public:
    BuddyTreeHeap buddy_heap;
private:
    char* base_address;
    size_t index = 0;

public:
    BuddyAllocator(void* base_address);

    struct allocation_result_t {
        void* mem;
        uint8_t order;
    };

    static size_t estimate_memory_used(size_t address_range);

    allocation_result_t allocate(uint8_t order);
    void deallocate(void* addr, uint8_t order);
    Chunk* chunk_for(void* address);
    BuddyInfoHeap* heap_index(size_t idx);

private:
    static size_t calculate_nodes_for_max_order();
    static int8_t order_for(size_t bytes) noexcept;
    Chunk* chunk_at_index(size_t index);

    void initialize(size_t memory_available, char* memory_base);
    void create_tree_structure(BuddyNode* parent_node);

    template <typename T>
    T* alloc() {
        T* curr = reinterpret_cast<T*>(base_address + index);
        index += sizeof(T);
        *curr = T{};
        return curr;
    }
};
}  // namespace firefly::kernel::mm::buddy
#endif