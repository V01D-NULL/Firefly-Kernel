# Firefly-Kernel
Kernel for FireflyOS

## x64 demo:
![Firefly OS](docs/x64-progress.png)

### Clone the repo 
 * `git clone https://github.com/FireflyOS/Firefly-Kernel --recursive`
 * `cd Firefly-Kernel`

## Ubuntu 

```bash
sudo apt install meson ninja-build nasm xorriso qemu-system-x86 clang lld ovmf #For UEFI emulation only
```

Firefly OS uses the meson build system:
```bash
meson build --cross-file meson_config.txt 
cd build
ninja
```
Note: FireflyOS can be booted on both BIOS and UEFI