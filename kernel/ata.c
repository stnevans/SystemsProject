/**
** @file ata.c
**
** @author CSCI-452 class of 20215
**
** authors: Jacob Doll & Eric Chen
**
** description: This is the file that performs the functionality for the ATA driver
** Note: Much of the implementation is pulled from Jacob's phoenixos project 
** with his permission
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
** Name:  ata_delay
**
** This function performs a delay of about 400ns
**
** @param dev ATA device structure
**
** @return None
*/
static void ata_delay(ata_device_t *dev) {
    __inb(dev->ctl_register);
    __inb(dev->ctl_register);
    __inb(dev->ctl_register);
    __inb(dev->ctl_register);
}

/**
** Name:  ata_software_reset
**
** This function resets the software
**
** @param dev ATA device structure
**
** @return None
*/
static void ata_software_reset(ata_device_t *dev) {
    __outb(dev->ctl_register, 0x04);
    ata_delay(dev);
    __outb(dev->ctl_register, 0x00);
}

/**
** Name:  detect_device_ATA
**
** This function detects the device type
**
** @param dev ATA device structure
**
** @return which device type it is
*/
int32_t detect_device_ATA(ata_device_t *dev){
    ata_software_reset(dev);
    __outb(ATA_IO_REG_DRV_SEL(dev->io_register), 0xA0 | dev->slavebit << 4);
    ata_delay(dev);
    unsigned cl = __inb(ATA_IO_REG_CYL_LO(dev->io_register));
    unsigned ch = __inb(ATA_IO_REG_CYL_HI(dev->io_register));

    if (cl == 0x14 && ch == 0xEB) return ATA_DEV_ATAPI;
    if (cl == 0x69 && ch == 0x96) return ATA_DEV_SATAPI;
    if (cl == 0x00 && ch == 0x00) return ATA_DEV_ATA;
    if (cl == 0x3C && ch == 0xC3) return ATA_DEV_SATA;
    return ATA_DEV_UNKOWN;
}

/**
** Name:  read_sectors_ATA_PIO
**
** This function reads a sector from the disk
**
** @param lba    The logical base address where the sector is
** @param buffer A buffer to contain the information from the sector
** @param dev    ATA device structure
**
** @return Size
*/
int32_t read_sectors_ATA_PIO(uint32_t lba, uint8_t buffer, ata_device_t *dev){
    uint8_t read_cmd[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t status;
	int size;

    __outb(ATA_IO_REG_DRV_SEL(dev->io_register), 0xA0 | dev->slavebit << 4);
    ata_delay(dev);

    __outb(ATA_IO_REG_FEAT(dev->io_register), 0x00);
    __outb(ATA_IO_REG_LBA1(dev->io_register), 2048 & 0xFF);
    __outb(ATA_IO_REG_LBA2(dev->io_register), 2048 >> 8);
    __outb(ATA_IO_REG_CMD(dev->io_register), 0xA0);

    while (1) {
        status = __inb(ATA_IO_REG_STATUS(dev->io_register));
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
        __outw(dev->io_register, out_cmd[i]);
    }

    size = (((int) __inb(ATA_IO_REG_LBA2(dev->io_register))) << 8) | (int) (__inb(ATA_IO_REG_LBA1(dev->io_register)));

    while (1) {
        status = __inb(ATA_IO_REG_STATUS(dev->io_register));
        if ((status & 0x01)) return -1;
        if (!(status & 0x80) && (status & 0x08)) break;
    }

    insw(dev->io_register, buffer, size/2);

    while (1) {
        status = __inb(ATA_IO_REG_STATUS(dev->io_register));
        if ((status & 0x01)) return -1;
        if (!(status & 0x80) && (status & 0x40)) break;
    }

    return size;
}

