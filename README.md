# minimal_cpu_info

A simple Linux kernel module that logs basic CPU information:
- CPU architecture (e.g. x86, arm, arm64)
- Long word length (BITS_PER_LONG)
- Endianness (little or big)

## Building

1. Make sure you have the kernel headers and build tools:
   ```bash
   sudo apt-get install build-essential linux-headers-$(uname -r)
   ```
2. Build the module:
   ```bash
   make
   ```
   
## Usage

- Load the module:
  ```bash
  sudo insmod minimal_cpu_info.ko
  ```
- Check kernel logs for output:
  ```bash
  dmesg | tail
  ```
- Unload the module:
  ```bash
  sudo rmmod minimal_cpu_info
  ```

## License

Dual MIT/GPL
