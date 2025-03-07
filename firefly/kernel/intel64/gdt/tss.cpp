#include "firefly/intel64/cpu/cpu.hpp"
#include "firefly/intel64/gdt/gdt.hpp"

namespace firefly::kernel::core::tss {

inline void load_tss(SegmentSelector selector) {
    asm("ltr %%ax\n" ::"a"(selector));
}

void init(uint64_t stack) {
    Tss& tss = thisCpu().tss;
    gdt::setTssEntry(thisCpu().gdt, reinterpret_cast<uint64_t>(&tss), 0x20, 0x89);
	tss.RSP0 = stack;
    tss.IST1 = 0; // TODO: Implement me!
    load_tss(static_cast<SegmentSelector>(gdt::tssEntryOffset()));
}
}  // namespace firefly::kernel::core::tss