/**
** @file filesystem.c
**
** @author CSCI-452 class of 20215
**
** author: Eric Chen
**
** description: This file sets up the FAT32 filesystem including
** the Boot Record, FAT, and data area. Also provides support for
** some filesystem functions.
*/

#define SP_KERNEL_SRC

#include "common.h"
#include "filesystem.h"
#include "ata.h"
#include "lib.h"
#include "cio.h"

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

// Possible default values for clusters
uint32_t FAT_FREE_CLUSTER = 0x00000000;
uint32_t FAT_EOC = 0x0FFFFFF8;
uint32_t FAT_BAD_CLUSTER = 0x0FFFFFF7;

// ATA Master and Slave drives
static ata_device_t ata_primary_master = {.io_register = 0x1F0, .ctl_register = 0x3F6, .slavebit = 0};
static ata_device_t ata_primary_slave = {.io_register = 0x1F0, .ctl_register = 0x3F6, .slavebit = 1};
static ata_device_t ata_secondary_master = {.io_register = 0x170, .ctl_register = 0x376, .slavebit = 0};
static ata_device_t ata_secondary_slave = {.io_register = 0x170, .ctl_register = 0x376, .slavebit = 1};
ata_device_t dev;

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  read_bpb
**
** This function uses the ATA device driver to find the sector
** where the BIOS Parameter Block is and loads it into
** the filesystem.
**
** @param filesystem The main FAT32 filesystem structure
** @param bios_block The structure that will hold the BIOS Parameter Block
**                   from the disk sector
**
** @return 1 if the BPB was found and read, -1 if there was an issue
*/
int read_bpb(f32_t *filesystem, bpb_t *bios_block){
    __cio_puts("\nReading BIOS Parameter Block from disk...\n");
    // Finds and reads the sector where the Boot Record is from disk
    uint8_t sector0[SECTOR_SIZE];
    read_sectors_ATA_PIO(0, (uint8_t *) sector0, &dev);

    // If the boot record is successfully found then the Bootable partition 
    // signature should be 0xAA55 at offset 0x1FE(510). 
    if(sector0[510] != 0xAA55){
        __cio_puts("\nError: Wrong Sector Found. Abandoning File System set up\n");
        return -1;
    }

    // BIOS Parameter Block
    __memcpy(&bios_block->bytes_per_sector, &sector0[11], sizeof(sector0[11]));
    __memcpy(&bios_block->sectors_per_cluster, &sector0[13], sizeof(sector0[13]));
    __memcpy(&bios_block->reserved_sectors, &sector0[14], sizeof(sector0[14]));
    __memcpy(&bios_block->num_FAT, &sector0[16], sizeof(sector0[16]));
    __memcpy(&bios_block->num_root_dir, &sector0[17], sizeof(sector0[17]));
    __memcpy(&bios_block->total_sectors, &sector0[19], sizeof(sector0[19])); 
    __memcpy(&bios_block->media_descriptor_type, &sector0[21], sizeof(sector0[21]));
    __memcpy(&bios_block->num_sectors_per_FAT, &sector0[22], sizeof(sector0[22])); 
    __memcpy(&bios_block->num_sectors_per_track, &sector0[24], sizeof(sector0[24]));
    __memcpy(&bios_block->num_heads_media, &sector0[26], sizeof(sector0[26]));
    __memcpy(&bios_block->num_hidden_sectors, &sector0[28], sizeof(sector0[28]));
    __memcpy(&bios_block->large_sector_count, &sector0[32], sizeof(sector0[32])); 

    // Extended Boot Record for FAT32
    __memcpy(&bios_block->sectors_per_FAT32, &sector0[36], sizeof(sector0[36]));
    __memcpy(&bios_block->flags, &sector0[40], sizeof(sector0[40]));
    __memcpy(&bios_block->FAT_version_num, &sector0[42], sizeof(sector0[42]));
    __memcpy(&bios_block->root_dir_cluster_num, &sector0[44], sizeof(sector0[44]));
    __memcpy(&bios_block->sector_num_FSInfo, &sector0[48], sizeof(sector0[48]));
    __memcpy(&bios_block->sector_num_backup, &sector0[50], sizeof(sector0[50]));
    __memcpy(&bios_block->drive_num, &sector0[64], sizeof(sector0[64]));
    __memcpy(&bios_block->windows_flags, &sector0[65], sizeof(sector0[65]));
    __memcpy(&bios_block->signature, &sector0[66], sizeof(sector0[66]));
    __memcpy(&bios_block->volume_id, &sector0[67], sizeof(sector0[67]));

    return 1;
}

/**
** Name:  make_Filesystem
**
** This function sets up the FAT32 Filesystem by getting the BPB information
** from the disk, gets the approximate positions for where the clusters and
** sectors are, and sets up the File Allocation Table and its sectors
**
** @param None
**
** @return The set up FAT32 filesystem structure
*/
f32_t *make_Filesystem(){
    f32_t *filesystem;

    __cio_puts("Identifying ATA Drive...\n");

    // Figure out what ATA drive to use
    if (detect_device_ATA(&ata_primary_master) == ATA_DEV_ATAPI) {
        dev = ata_primary_master;
        __cio_puts("ATA Drive Identified: Primary Master\n");
    } 
    else if (detect_device_ATA(&ata_primary_slave) == ATA_DEV_ATAPI) {
        dev = ata_primary_slave;
        __cio_puts("ATA Drive Identified: Primary Slave\n");
    }
    // This should be the proper drive here (Secondary Master) 
    else if (detect_device_ATA(&ata_secondary_master) == ATA_DEV_ATAPI) {
        dev = ata_secondary_master;
        __cio_puts("ATA Drive Identified: Secondary Master\n");
    } 
    else if (detect_device_ATA(&ata_secondary_slave) == ATA_DEV_ATAPI) {
        dev = ata_secondary_slave;
        __cio_puts("ATA Drive Identified: Secondary Slave\n");
    }

    // Get information about BPB
    if(read_bpb(filesystem, &filesystem->bios_block) == -1){
        return NULL;
    }

    // Finds various information about where sectors begin and how large clusters are
    filesystem->FAT_begin_sector = filesystem->bios_block.reserved_sectors;
    filesystem->data_begin_sector = filesystem->bios_block.reserved_sectors + (filesystem->bios_block.num_FAT * filesystem->bios_block.sectors_per_FAT32);
    filesystem->current_cluster_pos = 0;

    // Set up the File Allocation Table
    uint32_t FAT_size = SECTOR_SIZE * filesystem->bios_block.sectors_per_FAT32;
    
    // Set up sectors
    for(uint32_t sector_count = 0; sector_count < filesystem->bios_block.sectors_per_FAT32; sector_count++){
        uint8_t this_sector[SECTOR_SIZE];
        read_sectors_ATA_PIO(filesystem->FAT_begin_sector + sector_count, (uint8_t *) this_sector, &dev);
        for (int i = 0; i < SECTOR_SIZE/4; i++){
            filesystem->FAT[sector_count * (SECTOR_SIZE/4) + i] = this_sector[i * 4];
        }

    }

    // Creates the simulated root directory and adds it to the filesystem
    filesystem->dir = create_dir(filesystem, filesystem->data_begin_sector);

    return filesystem;
}

/**
** Name:  end_Filesystem
**
** This function ends the FAT32 filesystem and cleans it up
**
** @param filesystem The filesystem structure to be cleaned
**
** @return None
*/
void end_Filesystem(f32_t *filesystem){
    uint32_t FAT_size = SECTOR_SIZE * filesystem->bios_block.sectors_per_FAT32;
    __memclr(filesystem->FAT, FAT_size);
    __memclr(filesystem, sizeof(filesystem));
}

/**
** Name:  dir_entry_t
**
** This function creates a new directory file
**
** @param new_name  the name of the file
** @param type      the file type of the new file
** @param attribute the given attribute for the file
** @param size      the size of the file
**
** @return The new file (dir_entry_t)
*/
dir_entry_t *create_file(char* new_name, char* type, uint8_t attribute, uint32_t size, uint32_t cluster_num){
    // Checks if file name and file type match 8.3 standard
    if(__strlen(new_name) > MAX_FILENAME){
        return -1;
    }
    if(__strlen(type) > MAX_FILETYPE){
        return -1;
    }
    
    // Creates the file
    dir_entry_t *new_file;
    __strcpy(new_file->name, new_name);
    __strcpy(new_file->extension, type);
    new_file->attributes = attribute;
    new_file->file_size = size;
    new_file->first_cluster_high_bytes = cluster_num >> 16;
    new_file->first_cluster_low_bytes = cluster_num & 0xFFFF;

    return new_file;
}

/**
** Name:  find_dir
**
** This function finds a directory from the root directory
** given a filename and returns where it is in the root directory
**
** @param filesystem the FAT32 filesystem
** @param filename   the name of the file being looked for
**
** @return the directory position in the root directory
*/
int find_dir(f32_t *filesystem, char* filename){
    int entry_count;
    for(entry_count = 0; entry_count < filesystem->dir->cluster_num; entry_count++){
        if((__strcmp(filesystem->dir->entries[entry_count].name, filename == 0))){
            return entry_count;
        }    
    }
    return -1;
}

/**
** Name:  dir_read
**
** This function finds a directory and reads it using a given cluster
**
** @param filesystem the FAT32 filesystem
** @param new_file   the file being looked for
** @param cluster    the cluster number where the file is located
**
** @return None
*/
uint32_t *dir_read(f32_t *filesystem, char *filename){
    // Calculates information about where the file is and the sectors for the cluster
    uint32_t root_cluster = filesystem->bios_block.root_dir_cluster_num;
    dir_entry_t *this_file = &filesystem->dir->entries[find_dir(filesystem, filename)];
    uint32_t first_sector_of_cluster = ((this_file->first_cluster_low_bytes - 2) * filesystem->bios_block.sectors_per_cluster) + filesystem->data_begin_sector;
    uint32_t current_cluster = this_file->first_cluster_low_bytes;
    uint32_t current_cluster_sector = first_sector_of_cluster;

    // Prepares the storage of the data of the entry
    uint32_t *entry;
    uint32_t *entry_ptr = entry;
    uint8_t entry_buffer[32];

    // Gets the first byte of the entry
    uint32_t first_byte = current_cluster_sector & 0xFF;

    // Looks through the cluster chain for the entire file
    while(current_cluster != FAT_EOC){
        // If the first byte is 0 then there are no more entries in this cluster
        while(first_byte != 0){
            // If the first byte is 0xE5 then the entry is unused and we move onto the next entry
            if (first_byte != 0xE5){
                // Read current entry
                read_sectors_ATA_PIO(current_cluster_sector, (uint8_t *) entry_buffer, &dev);
                // Copies it into the entry and updates the pointer
                __memcpy(entry_ptr, entry_buffer, sizeof(entry_buffer));
                // Goes to the next entry on the sector
                current_cluster_sector += 32;
                entry_ptr += 32;
            }
            // Updates the first byte
            first_byte += 32;
        }
        
        // Iterates through cluster chain
        uint32_t fat_offset = current_cluster * 4;
        uint32_t fat_sector = current_cluster_sector + (fat_offset / SECTOR_SIZE);
        uint32_t ent_offset = fat_offset % SECTOR_SIZE;
        current_cluster = *(uint32_t *)&filesystem->FAT[ent_offset];
        current_cluster_sector = ((current_cluster - 2) * filesystem->bios_block.sectors_per_cluster) + filesystem->data_begin_sector;
        first_byte = current_cluster_sector & 0xFF;
    }
    
    return entry;
}

/**
** Name:  file_write
**
** This function adds a new file to a directory and to the FAT
**

** @param filesystem the FAT32 filesystem
** @param new_file   the file to be added to the filesystem
** @param this_dir   the directory where the file will be added to
**
** @return None
*/
void file_write(f32_t *filesystem, dir_entry_t *new_file){
    // Finds first free entry in directory
    int empty_entry;
    for(empty_entry = 0; empty_entry < SECTOR_SIZE; empty_entry += sizeof(dir_entry_t)){
        if((&filesystem->dir->entries[empty_entry] == 0)){
            break;
        }    
    }
    
    // Add file to directory
    __memcpy(&filesystem->dir->entries[empty_entry], new_file, sizeof(dir_entry_t));
    filesystem->dir->num_entries++;
    
    // Writes cluster's address into directory entry
    new_file->first_cluster_low_bytes = filesystem->current_cluster_pos;

    // Adds file to FAT
    filesystem->FAT[filesystem->current_cluster_pos] = FAT_EOC;
    filesystem->current_cluster_pos += 32;
}

/**
** Name:  delete_file
**
** This function removes a file from the filesystem and the directory
** it is in
**
** @param filesystem the FAT32 filesystem
** @param del_file   the file to be deleted
** @param this_dir   the directory this file is being removed from
**
** @return None
*/
void delete_file(f32_t *filesystem, dir_entry_t *del_file){
    // Finds file in directory and remove it
    int entry_position;
    for(entry_position = 0; entry_position < SECTOR_SIZE; entry_position+=sizeof(dir_entry_t)){
        if((&filesystem->dir->entries[entry_position] == 0)){
            break;
        }    
    }
    __memclr(&filesystem->dir->entries[entry_position], sizeof(dir_entry_t));
    filesystem->dir->num_entries--;

    // Removes file from FAT by setting cluster chain to zero
    filesystem->FAT[del_file->first_cluster_low_bytes] = FAT_FREE_CLUSTER;
}

/**
** Name:  create_dir
**
** This function creates a new directory
**
** @param filesystem the FAT32 filesystem
** @param cluster    the cluster where the directory will be held
**
** @return The newly created directory (directory_t)
*/
directory_t *create_dir(f32_t *filesystem, uint32_t cluster){
    directory_t *new_dir;
    new_dir->cluster_num = cluster;
    new_dir->num_entries = 0;

    return new_dir;
}

/**
** Name:  rm_dir
**
** This function clears out a directory
**
** @param filesystem the FAT32 filesystem
** @param dir        the directory to be cleaned and removed
**
** @return None
*/
void rm_dir(f32_t *filesystem, directory_t *dir){
    for(int i = 0; i < dir->num_entries; i++){
        __memclr(&dir->entries[i], sizeof(dir->entries[i]));
    }

    __memclr(dir->entries, dir->num_entries);
}
