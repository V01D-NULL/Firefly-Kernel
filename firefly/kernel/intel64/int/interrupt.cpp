#include "firefly/intel64/int/interrupt.hpp"

#include <cstdint>

#include "cstdlib/cassert.h"
#include "firefly/drivers/ports.hpp"
#include "firefly/intel64/cpu/apic/apic.hpp"
#include "firefly/logger.hpp"
#include "firefly/panic.hpp"
#include "firefly/trace/symbols.hpp"

namespace firefly::kernel::core::interrupt {
struct __attribute__((packed)) idt_gate {
    uint16_t offset_0;
    uint16_t selector;
    uint8_t rsv_0;
    uint8_t type;
    uint16_t offset_1;
    uint32_t offset_2;
    uint32_t rsv_1;
};

static_assert(16 == sizeof(idt_gate), "idt_gate size incorrect");

extern "C" {
void interrupt_handler(InterruptFrame iframe);
InterruptFrame* irq_handler(InterruptFrame* iframe);
void exception_handler([[maybe_unused]] InterruptFrame iframe);
void interrupt_wrapper();
void exception_wrapper();
void assign_cpu_exceptions();
void assign_irq_handlers();
}

static idt_gate idt[256];

static const char* exceptions[] = {
    "Divide by Zero",
    "Debug",
    "NMI",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device not available",
    "Double Fault",
    "",
    "Invalid TSS",
    "Segment not present",
    "Stack Segment fault",
    "GPF",
    "Page Fault",
    "",
    "x87 Floating point exception",
    "Alignment check",
    "Machine check",
    "SIMD Floating point exception",
    "Virtualization Exception",
    "Control protection exception",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor injection exception",
    "VMM Communication exception",
    "Security exception",
    ""
};

namespace change {
extern "C" void update(void (*handler)(), uint16_t cs, uint8_t type, uint8_t index) {
    idt[index].offset_0 = reinterpret_cast<size_t>(handler) & 0xffff;
    idt[index].selector = cs;
    idt[index].rsv_0 = 0;
    idt[index].type = type;
    idt[index].offset_1 = reinterpret_cast<size_t>(handler) >> 16 & 0xffff;
    idt[index].offset_2 = reinterpret_cast<size_t>(handler) >> 32 & 0xffffffff;
    idt[index].rsv_1 = 0;
}
}  // namespace change

struct __attribute__((packed)) idt_reg {
    /**
     *                  size of table in bytes - 1
     */
    uint16_t limit;
    /**
     *                  base address of idt
     */
    idt_gate* base;
} idtr = {
    .limit = (sizeof(struct idt_gate) * 256) - 1,
    .base = idt
};

void init() {
    assign_cpu_exceptions();
    assign_irq_handlers();

    asm("lidt %0" ::"m"(idtr)
        : "memory");
}

void interrupt_handler(InterruptFrame iframe) {
    logLine << "Exception: " << exceptions[iframe.int_no] << "\n"
            << fmt::endl;
    logLine << "Int#: " << iframe.int_no << "\nError code: " << iframe.err << fmt::endl;
    logLine << "RIP: " << fmt::hex << iframe.rip << fmt::endl;

    debugLine << "Rip: " << fmt::hex << iframe.rip << '\n'
              << "Rax: " << fmt::hex << iframe.rax << '\n'
              << "Rbx: " << fmt::hex << iframe.rbx << '\n'
              << "Rcx: " << fmt::hex << iframe.rcx << '\n'
              << "Rdx: " << fmt::hex << iframe.rdx << '\n'
              << "Rdi: " << fmt::hex << iframe.rdi << '\n'
              << "Rsi: " << fmt::hex << iframe.rsi << '\n'
              << "R8: " << fmt::hex << iframe.r8 << '\n'
              << "R9: " << fmt::hex << iframe.r9 << '\n'
              << "R10: " << fmt::hex << iframe.r10 << '\n'
              << "R11: " << fmt::hex << iframe.r11 << '\n'
              << "R12: " << fmt::hex << iframe.r12 << '\n'
              << "R13: " << fmt::hex << iframe.r13 << '\n'
              << "R14: " << fmt::hex << iframe.r14 << '\n'
              << "R15: " << fmt::hex << iframe.r15 << '\n'
              << fmt::endl;

    panic("interrupt");

    for (;;)
        asm("cli\nhlt");
}

using InterruptHandlerCallback = void (*)(ContextRegisters*);
static frg::array<InterruptHandlerCallback, 24> irqHandlers;

void registerIRQHandler(void (*handler)(ContextRegisters*), uint8_t irq) {
    assert_truth(irqHandlers[irq] == nullptr && "Tried to overwrite IRQ handler");
    irqHandlers[irq] = handler;
}

void unregisterIRQHandler(uint8_t irq) {
    irqHandlers[irq] = nullptr;
}

InterruptFrame* irq_handler(InterruptFrame* stack) {
    uint8_t irq = stack->int_no - apic::LVT_BASE;
    if (irqHandlers[irq] != nullptr) {
        irqHandlers[irq](stack);
    } else {
        debugLine << "Unhandled IRQ received! IRQ #" << stack->int_no - apic::LVT_BASE << "\n"
                  << fmt::endl;
    }
    apic::Apic::accessor().sendEOI();
    return stack;
}

}  // namespace firefly::kernel::core::interrupt
