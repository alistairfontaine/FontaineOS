#!/bin/bash

# Clear the terminal display window frame
clear

echo "===================================================="
echo "⚡ FONTAINE OS UNIVERSAL DEPLOYMENT & GIT ENGINE ⚡"
echo "===================================================="

# 1. Sweep out older intermediate binary files
echo "🧹 Scrubbing old object assets..."
make clean

# 2. Check if a valid disk image exists. If missing or fresh, create a properly initialized one
if [ ! -f bin/disk.img ]; then
    echo "📦 Allocating initialized virtual storage drive disk space..."
    mkdir -p bin
    dd if=/dev/zero of=bin/disk.img bs=512 count=20480 2>/dev/null
    # Write a quick MBR structural signature placeholder to block 0 so the controller pins activate cleanly
    printf '\x55\xAA' | dd of=bin/disk.img bs=1 seek=510 conv=notrunc 2>/dev/null
fi

# 3. Trigger the primary project compilation pipeline
echo "🔨 Executing core compiler and linker stages..."
make bin/fontaineos.bin

# 4. Verify if the kernel compiled successfully
if [ $? -eq 0 ]; then
    echo "✅ Compilation successful! FontaineOS Image Secured."

    # Create a timestamped historical backup point inside bin/
    TIMESTAMP=$(date +%Y%m%d_%H%M%S)
    cp bin/fontaineos.bin bin/fontaineos_backup_${TIMESTAMP}.bin
    echo "📦 Saved historical kernel state backup: fontaineos_backup_${TIMESTAMP}.bin"

    # 5. Automate the remote repository deployment tracks
    echo "🌐 Syncing production code updates straight to GitHub..."
    git add .
    git commit -m "feat: master pipeline automation build cycle execution at ${TIMESTAMP}"
    git push origin main

    # 6. Boot up the hardware virtualization display console using raw explicit controllers
    echo "🚀 Booting FontaineOS inside QEMU Emulation Core..."
    qemu-system-i386 -kernel bin/fontaineos.bin -drive file=bin/disk.img,format=raw,index=0,media=disk
else
    echo "❌ CRITICAL: Compilation failed. Aborting Git tracking and execution steps."
    exit 1
fi
