/**
** @file ata.h
**
** @author CSCI-452 class of 20215
**
** authors: Jacob Doll & Eric Chen
**
** description: Header file for ATA driver
** Note: Much of the implementation is pulled from Jacob's phoenixos 
** project with his permission
*/

#ifndef _ATA_H_
#define _ATA_H_

#include "common.h"
#include "lib.h"

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

#ifndef SP_ASM_SRC

/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
*/

#define ATA_IO_REG_DATA(x)        (x + 0x0)
#define ATA_IO_REG_ERROR(x)       (x + 0x1)
#define ATA_IO_REG_FEAT(x)        (x + 0x1)
#define ATA_IO_REG_SECT_COUNT(x)  (x + 0x2)
#define ATA_IO_REG_SECT_NUM(x)    (x + 0x3)
#define ATA_IO_REG_LBA0(x)        (x + 0x3)
#define ATA_IO_REG_CYL_LO(x)      (x + 0x4)
#define ATA_IO_REG_LBA1(x)        (x + 0x4)
#define ATA_IO_REG_CYL_HI(x)      (x + 0x5)
#define ATA_IO_REG_LBA2(x)        (x + 0x5)
#define ATA_IO_REG_DRV_SEL(x)     (x + 0x6)
#define ATA_IO_REG_STATUS(x)      (x + 0x7)
#define ATA_IO_REG_CMD(x)         (x + 0x7)

#define ATA_CTL_REG_ALT_STATUS 0x0
#define ATA_CTL_REG_DEV_CTL    0x0
#define ATA_CTL_REG_DRV_ADDR   0x1

#define ATA_DEV_UNKOWN 0x00
#define ATA_DEV_ATA    0x01
#define ATA_DEV_ATAPI  0x02
#define ATA_DEV_SATA   0x03
#define ATA_DEV_SATAPI 0x04

#define ATAPI_SECTOR_SIZE 2048

/*
** Types
*/

typedef struct ata_device{
    int32_t io_register;
    int32_t ctl_register;
    int32_t slavebit;

} ata_device_t;

/*
** Globals
*/

/*
** Prototypes
*/

int32_t detect_device_ATA(ata_device_t *dev);
int32_t read_sectors_ATA_PIO(uint32_t lba, uint8_t buffer, ata_device_t *dev);
int32_t write_sectors_ATA_PIO(uint32_t lba, uint8_t sector_count, uint32_t *bytes);

#endif
/* SP_ASM_SRC */

#endif
