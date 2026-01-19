# Kiwi Kernel (KIWI+Essentials)

Brand new Kernel with a built in shell and RAM DISK File System, KIWI+Essentials.

Before starting, make sure you have Homebrew installed. If not, install from https://brew.sh.

# Build in macOS

Open a Terminal at the build directory

Run "brew install aarch64-elf-gcc aarch64-elf-binutils" in Terminal.

Run "i686-elf-gcc -ffreestanding -nostdlib -c boot.S -o boot.o
i686-elf-gcc -ffreestanding -nostdlib -c kernel.c -o kernel.o
i686-elf-ld -n -T linker.ld boot.o kernel.o -o kernel.elf" in Terminal.

# Running in macOS

Open a Terminal where your kernel.elf is located.

Run "brew install qemu" in Terminal.

Run "qemu-system-i386 -kernel kernel.elf" in Terminal.

# Commiting
If you would like to make a commit, go ahead and make a pull request or a fork!
