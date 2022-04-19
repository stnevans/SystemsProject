/**
** @file ata.c
**
** @author CSCI-452 class of 20215
**
** author: Jacob Doll & Eric Chen
**
** description:
*/

#define	SP_KERNEL_SRC

#include "common.h"
#include "ata.h"
#include "ports.h"
#include "lib.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
static void ata_delay(ata_device_t *dev) {
    inb(dev->ctl_register);
    inb(dev->ctl_register);
    inb(dev->ctl_register);
    inb(dev->ctl_register);
}

static void ata_software_reset(ata_device_t *dev) {
    outb(dev->ctl_register, 0x04);
    ata_delay(dev);
    outb(dev->ctl_register, 0x00);
}

int32_t detect_device_ATA(ata_device_t *dev){
    ata_software_reset(dev);
    outb(ATA_IO_REG_DRV_SEL(dev->io_register), 0xA0 | dev->slavebit << 4);
    ata_delay(dev);
    unsigned cl = inb(ATA_IO_REG_CYL_LO(dev->io_register));
    unsigned ch = inb(ATA_IO_REG_CYL_HI(dev->io_register));

    if (cl == 0x14 && ch == 0xEB) return ATA_DEV_ATAPI;
    if (cl == 0x69 && ch == 0x96) return ATA_DEV_SATAPI;
    if (cl == 0x00 && ch == 0x00) return ATA_DEV_ATA;
    if (cl == 0x3C && ch == 0xC3) return ATA_DEV_SATA;
    return ATA_DEV_UNKOWN;
}


int32_t read_sectors_ATA_PIO(uint32_t lba, uint8_t buffer, ata_device_t *dev){
    uint8_t read_cmd[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t status;
	int size;

    outb(ATA_IO_REG_DRV_SEL(dev->io_register), 0xA0 | dev->slavebit << 4);
    ata_delay(dev);

    outb(ATA_IO_REG_FEAT(dev->io_register), 0x00);
    outb(ATA_IO_REG_LBA1(dev->io_register), 2048 & 0xFF);
    outb(ATA_IO_REG_LBA2(dev->io_register), 2048 >> 8);
    outb(ATA_IO_REG_CMD(dev->io_register), 0xA0);

    while (1) {
        status = inb(ATA_IO_REG_STATUS(dev->io_register));
        if ((status & 0x01)) return -1;
		if (!(status & 0x80) && (status & 0x08)) break;
    }

	read_cmd[2] = (lba >> 0x18) & 0xFF;
	read_cmd[3] = (lba >> 0x10) & 0xFF;
	read_cmd[4] = (lba >> 0x08) & 0xFF;
	read_cmd[5] = (lba >> 0x00) & 0xFF;
    read_cmd[9] = 1;

    uint16_t* out_cmd = (uint16_t*) read_cmd;
    for (int i = 0; i < 6; i++) {
        outw(dev->io_register, out_cmd[i]);
    }

    size = (((int) inb(ATA_IO_REG_LBA2(dev->io_register))) << 8) | (int) (inb(ATA_IO_REG_LBA1(dev->io_register)));

    while (1) {
        status = inb(ATA_IO_REG_STATUS(dev->io_register));
        if ((status & 0x01)) return -1;
        if (!(status & 0x80) && (status & 0x08)) break;
    }

    insw(dev->io_register, buffer, size/2);

    while (1) {
        status = inb(ATA_IO_REG_STATUS(dev->io_register));
        if ((status & 0x01)) return -1;
        if (!(status & 0x80) && (status & 0x40)) break;
    }

    return size;
}

//TODO
int32_t write_sectors_ATA_PIO(uint32_t lba, uint8_t sector_count, uint32_t *bytes){
    
    

}
