# Firefly-Kernel
Kernel for FireflyOS which can be booted on UEFI and BIOS

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
make -C limine/ # Make sure you build the limine-install binary
meson build --cross-file meson_config.toml # You *must* use build, other scripts depend on this directory name
cd build
meson compile && ../Scripts/geniso.sh && ../Scripts/qemu-bios.sh # If meson compile is not supported you can either upgrade meson or use ninja
```
Note: It is assumed you have meson version `0.60.1` or higher, you may or may not run into problems with older versions.