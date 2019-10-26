#include <interrupt.hpp>
#include <drivers/ps2.hpp>
#include <drivers/vga.hpp>
#include <stl/array.h>
#include <multiboot.hpp>

[[maybe_unused]] constexpr short MAJOR_VERSION = 0;
[[maybe_unused]] constexpr short MINOR_VERSION = 0;
constexpr const char* VERSION_STRING = "0.0";

void write_ff_info(cursor &crs) {
    vga::clear();
    crs << "FireflyOS\nVersion: " << VERSION_STRING << "\nContributors:";

    firefly::std::array<const char*, 7> arr = {
        "Lime\t  ", "JohnkaS", "EyeDevelop", "4lpha", "Burokkoru ", "extation", "RedEye2D"
    };

    for (size_t i = 0; i < arr.max_size(); i++) {
        if (i % 2 == 0) {
            crs << "\n\t";
        }
        crs << arr[i] << "  ";
    }
    crs << "\n";
}

/**
 *                          Long mode main kernel entry point
 * @param  mboot_info_addr  physical address of the multiboot information structure
 */
extern "C" [[noreturn]] void kernel_main(uintptr_t mboot_info_addr) {
    vga::init();
    ps2::init();
    interrupt::init();

    cursor crs{ color::white, color::black, 0, 0 };

    write_ff_info(crs);

    // eh
    start_load(crs, "Loading VGA driver");
    end_load(crs, "Loaded VGA driver");
    start_load(crs, "Loading PS/2 driver");
    end_load(crs, "Loaded PS/2 driver");

    while (true) {
        auto key = ps2::get_scancode();
        if (!key.has_value()) {
            continue;
        }
        ps2::handle_input(*key, crs);
    }
}