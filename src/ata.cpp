#include "ata.h"
#include "timer.h"

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

inline void insw(uint16_t port, void* addr, uint32_t count) {
    asm volatile (
        "cld\n\t"
        "rep insw"
        : "+D"(addr), "+c"(count)
        : "d"(port)
        : "memory"
    );
}

// Static global hardware sector landing pad to completely avoid stack range collisions
static uint8_t ata_io_buffer[512] __attribute__((aligned(4)));

/*
   Waits until the motherboard hard disk controller clears its Busy bit (BSY)
   and raises its Data Request Ready flag (DRQ).
*/
static void ata_wait_ready() {
    while (inb(ATA_STATUS) & ATA_STATUS_BSY) {
        asm volatile("" : : : "memory");
    }
    while (!(inb(ATA_STATUS) & ATA_STATUS_DRQ)) {
        asm volatile("" : : : "memory");
    }
}

extern "C" void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    outb(ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_FEATURES, 0x00);
    outb(ATA_SECTOR_CNT, 0x01);
    outb(ATA_LBA_LOW,  (uint8_t)(lba & 0xFF));
    outb(ATA_LBA_MID,  (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));
    outb(ATA_COMMAND, ATA_CMD_READ);

    ata_wait_ready();

    // Stream the raw hardware sector straight into our safe global binary buffer area
    insw(ATA_DATA, ata_io_buffer, 256);

    // Safely replicate the landed bytes back out into the target function container
    for (int i = 0; i < 512; i++) {
        buffer[i] = ata_io_buffer[i];
    }
}

extern "C" void ata_write_sector(uint32_t lba, const uint8_t* buffer) {
    auto outsw = [](uint16_t port, const void* addr, uint32_t count) {
        asm volatile (
            "cld\n\t"
            "rep outsw"
            : "+S"(addr), "+c"(count)
            : "d"(port)
            : "memory"
        );
    };

    // Snapshot the incoming data parameters into our flat global hardware platform segment
    for (int i = 0; i < 512; i++) {
        ata_io_buffer[i] = buffer[i];
    }

    outb(ATA_DRIVE_HEAD, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_SECTOR_CNT, 0x01);
    outb(ATA_LBA_LOW,  (uint8_t)(lba & 0xFF));
    outb(ATA_LBA_MID,  (uint8_t)((lba >> 8) & 0xFF));
    outb(ATA_LBA_HIGH, (uint8_t)((lba >> 16) & 0xFF));

    // Send the write token command
    outb(ATA_COMMAND, ATA_CMD_WRITE);

    /*
       Fixed Sequence:
       We must feed the data words onto the bus pins immediately *before*
       waiting, unblocking the controller's internal write cache matrix!
    */
    outsw(ATA_DATA, ata_io_buffer, 256);

    // Now wait for the storage hardware to finish flushing the cache onto the plates
    ata_wait_ready();
}
