# Define our compiler tools and target architecture flags
AS = nasm
CC = g++
LD = ld

# Freestanding C++ target compilation flags matrix
ASFLAGS = -f elf32
CFLAGS  = -m32 -c -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -Iinclude -std=c++20
LDFLAGS = -m elf_i386 -T linker.ld

all: bin/fontaineos.bin

bin/fontaineos.bin: src/boot.o src/kernel.o
	$(LD) $(LDFLAGS) -o bin/fontaineos.bin src/boot.o src/kernel.o

src/boot.o: src/boot.s
	$(AS) $(ASFLAGS) src/boot.s -o src/boot.o

src/kernel.o: src/kernel.cpp
	$(CC) $(CFLAGS) src/kernel.cpp -o src/kernel.o

run: bin/fontaineos.bin
	qemu-system-i386 -kernel bin/fontaineos.bin

clean:
	rm -f src/*.o bin/*.bin
