#include <stl/array.h>
#include <stl/cstdlib/stdio.h>
#include "x86_64/libk++/iostream.h"

#include "x86_64/init/init.hpp"
#include "x86_64/drivers/ps2.hpp"
#include "x86_64/drivers/serial.hpp"
#include "x86_64/drivers/vbe.hpp"
#include "x86_64/kernel.hpp"
#include "x86_64/trace/strace.hpp"
#include "x86_64/memory-manager/primary/primary_phys.hpp"
#include "x86_64/memory-manager/virtual/virtual.hpp"
#include "x86_64/gdt/tss.hpp"
#include "x86_64/uspace/userspace.hpp"
#include <frg/vector.hpp>
#include <frg/list.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char *VERSION_STRING = "0.0";

namespace firefly::kernel::main {
void write_ff_info() {
    printf("FireflyOS\nVersion: %s\nContributors:", VERSION_STRING);

    firefly::std::array<const char *, 3> arr = {
        "Lime\t  ", "JohnkaS", "V01D-NULL"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            puts("\n\t");
        }
        printf("%s  ", arr[i]);
    }
    puts("\n");
}

class Alloc
{
    public:
        void *allocate(size_t sz) {
            (void)sz;
            return firefly::kernel::mm::primary::allocate(1)->data[0];
        }

        void free(void *ptr)
        {
            (void)ptr;
        }
};

struct LIST {
    frg::default_list_hook<LIST> next;
    int a;
};

[[noreturn]] void kernel_main(stivale2_struct *handover) {
    // Never free rsp0
    auto rsp0 = firefly::kernel::mm::primary::allocate(1);
    if (rsp0 == nullptr) firefly::trace::panic("Failed to allocate memory for the TSS for core 0 (main core)");
    firefly::kernel::core::tss::core0_tss_init(reinterpret_cast<size_t>(rsp0->data[0]));
    
    auto tagmem = static_cast<stivale2_struct_tag_memmap *>(stivale2_get_tag(handover, STIVALE2_STRUCT_TAG_MEMMAP_ID));
    mm::VirtualMemoryManager vmm{true, tagmem};

    // Frigg port PoC / demo
    frg::vector<int, Alloc> vec;
    printf("%x\n", vec.size());

    frg::intrusive_list<
        LIST,
        frg::locate_member<
            LIST,
            frg::default_list_hook<LIST>,
            &LIST::next            
        >
    > list;
    
    LIST *l = (LIST*)mm::primary::allocate(1)->data[0];
    l->a = 12;
    list.push_back(l);
    printf("Is list empty: %s\n", list.empty() ? "yes" : "no");
    printf("list.pop_back().a: %d\n", list.pop_back()->a);

    trace::panic("Reached the end of kernel");
    __builtin_unreachable();
}
}  // namespace firefly::kernel::main