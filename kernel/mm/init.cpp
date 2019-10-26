#include <mm/init.hpp>
#include <mm/page.hpp>

bool mm::init() {
    // this function will check all of issue #4's boxes

    // - take physical address of multiboot info structure and
    //   map it to some virtual space
    //   (will require some basic vmm functionality)
    // - read the info for the memory map, while
    //   building "zones" of the free regions into memory
    // - unmap the multiboot info structure

    return true;
}