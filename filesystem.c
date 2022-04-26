/**
** @file filesystem.c
**
** @author CSCI-452 class of 20215
**
** author: Eric Chen
**
** description:
*/

#define SP_KERNEL_SRC

#include "common.h"
#include "filesystem.h"
#include "ata.h"
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

// Possible default values for clusters
uint32_t FAT_FREE_CLUSTER = 0x00000000;
uint32_t FAT_EOC = 0x0FFFFFF8;
uint32_t FAT_BAD_CLUSTER = 0x0FFFFFF7;


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
** @return None
*/
void read_bpb(f32_t *filesystem, bpb_t *bios_block){
    // Finds and reads the sector where the Boot Record is from disk
    uint8_t sector0[512];
    ata_device_t dev = {.io_register = 0x1F0, .ctl_register = 0x3F6, .slavebit = 0};
    read_sectors_ATA_PIO(0, sector0, &dev);

    // BIOS Parameter Block
    __memcpy(bios_block->bytes_per_sector, sector0[11], sizeof(sector0[11]));
    __memcpy(bios_block->sectors_per_cluster, sector0[13], sizeof(sector0[13]));
    __memcpy(bios_block->reserved_sectors, sector0[14], sizeof(sector0[14]));
    __memcpy(bios_block->num_FAT, sector0[16], sizeof(sector0[16]));
    __memcpy(bios_block->num_root_dir, sector0[17], sizeof(sector0[17]));
    __memcpy(bios_block->total_sectors, sector0[19], sizeof(sector0[19])); 
    __memcpy(bios_block->media_descriptor_type, sector0[21], sizeof(sector0[21]));
    __memcpy(bios_block->num_sectors_per_FAT, sector0[22], sizeof(sector0[22])); 
    __memcpy(bios_block->num_sectors_per_track, sector0[24], sizeof(sector0[24]));
    __memcpy(bios_block->num_heads_media, sector0[26], sizeof(sector0[26]));
    __memcpy(bios_block->num_hidden_sectors, sector0[28], sizeof(sector0[28]));
    __memcpy(bios_block->large_sector_count, sector0[32], sizeof(sector0[32])); 

    // Extended Boot Record for FAT32
    __memcpy(bios_block->sectors_per_FAT32, sector0[36], sizeof(sector0[36]));
    __memcpy(bios_block->flags, sector0[40], sizeof(sector0[40]));
    __memcpy(bios_block->FAT_version_num, sector0[42], sizeof(sector0[42]));
    __memcpy(bios_block->root_dir_cluster_num, sector0[44], sizeof(sector0[44]));
    __memcpy(bios_block->sector_num_FSInfo, sector0[48], sizeof(sector0[48]));
    __memcpy(bios_block->sector_num_backup, sector0[50], sizeof(sector0[50]));
    __memcpy(bios_block->drive_num, sector0[64], sizeof(sector0[64]));
    __memcpy(bios_block->windows_flags, sector0[65], sizeof(sector0[65]));
    __memcpy(bios_block->signature, sector0[66], sizeof(sector0[66]));
    __memcpy(bios_block->volume_id, sector0[67], sizeof(sector0[67]));
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
    __memclr(filesystem, sizeof(f32_t));

    // Get information about BPB
    read_bpb(filesystem, &filesystem->bios_block);

    // Finds various information about where sectors begin and how large clusters are
    filesystem->partition_begin_sector = 0;
    filesystem->FAT_begin_sector = filesystem->partition_begin_sector + filesystem->bios_block.reserved_sectors;
    filesystem->cluster_begin_sector = filesystem->FAT_begin_sector + (filesystem->bios_block.num_FAT * filesystem->bios_block.sectors_per_FAT32);
    filesystem->cluster_size = 512 * filesystem->bios_block.sectors_per_cluster;
    filesystem->current_cluster_pos = 0;

    // Set up the File Allocation Table
    uint32_t FAT_size = 512 * filesystem->bios_block.sectors_per_FAT32;
    __memclr(filesystem->FAT, FAT_size);
    
    // Set up sectors
    for(uint32_t sector_count = 0; sector_count < filesystem->bios_block.sectors_per_FAT32; sector_count++){
        uint32_t this_sector[512];
        ata_device_t dev = {.io_register = 0x1F0, .ctl_register = 0x3F6, .slavebit = 0};
        read_sectors_ATA_PIO(filesystem->FAT_begin_sector + sector_count, this_sector, &dev);
        for (int i = 0; i < 512/4; i++){
            filesystem->FAT[sector_count * (512/4) + i] = this_sector[i * 4];
        }

    }

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
    //free(filesystem->FAT);
    //free(filesystem);
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
dir_entry_t *create_file(char* new_name, char* type, uint8_t attribute, uint32_t size){
    // Checks if file name and file type match 8.3 standard
    if(strlen(new_name) > MAX_FILENAME){
        return -1;
    }
    if(strlen(type) > MAX_FILETYPE){
        return -1;
    }
    
    // Creates the file
    dir_entry_t *new_file;
    __memclr(new_file, sizeof(dir_entry_t));
    __strcpy(new_file->name, new_name);
    __strcpy(new_file->extension, type);
    new_file->attributes = attribute;
    new_file->file_size = size;

    return new_file;
}

/**
** Name:  file_read
**
** This function finds a file and reads it using a given cluster
**
** @param filesystem the FAT32 filesystem
** @param new_file   the file being looked for
** @param cluster    the cluster number where the file is located
**
** @return None
*/
void file_read(f32_t *filesystem, dir_entry_t *new_file, uint32_t cluster){
    // Calculates information about where the file is and the sectors for the cluster
    uint32_t root_cluster = filesystem->bios_block.root_dir_cluster_num;
    uint32_t first_sector_of_cluster = ((cluster - 2) * filesystem->bios_block.sectors_per_cluster) + filesystem->partition_begin_sector;
    uint32_t current_cluster = cluster;
    uint32_t current_cluster_sector = first_sector_of_cluster;
    uint32_t current_byte = __inb(first_sector_of_cluster);
    
    // Looks through the cluster chain for the entire file
    while(filesystem->FAT[current_cluster] != FAT_EOC){
        while(current_byte != 0){
            // Read current entry

            current_byte = __inb(current_cluster_sector);
        }
        
        // Iterates through cluster chain
        current_cluster = filesystem->FAT[current_cluster];
        current_cluster_sector = ((current_cluster - 2) * filesystem->bios_block.sectors_per_cluster) + filesystem->partition_begin_sector;
        current_byte = __inb(current_cluster_sector);
    }
    
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
void file_write(f32_t *filesystem, dir_entry_t *new_file, directory_t *this_dir){
    // Finds first free entry in directory
    int empty_entry;
    for(empty_entry = 0; empty_entry < filesystem->cluster_size; empty_entry+=sizeof(dir_entry_t)){
        if((&this_dir->entries[empty_entry] == 0)){
            break;
        }    
    }
    
    // Add file to directory
    __memcpy(&this_dir->entries[empty_entry], new_file, sizeof(dir_entry_t));
    this_dir->num_entries++;
    
    // Adds file to FAT
    filesystem->FAT[filesystem->current_cluster_pos] = FAT_EOC;
    filesystem->current_cluster_pos += filesystem->cluster_size;

    // Writes cluster's address into directory entry
    new_file->first_cluster_low_bytes = filesystem->current_cluster_pos;

    //TODO: Add functionality for cluster chain


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
void delete_file(f32_t *filesystem, dir_entry_t *del_file, directory_t *this_dir){
    // Finds file in directory and remove it
    int entry_position;
    for(entry_position = 0; entry_position < filesystem->cluster_size; entry_position+=sizeof(dir_entry_t)){
        if((&this_dir->entries[entry_position] == 0)){
            break;
        }    
    }
    __memclr(&this_dir->entries[entry_position], sizeof(dir_entry_t));
    this_dir->num_entries--;

    // Removes file from FAT by setting cluster chain to zero
    filesystem->FAT[del_file->first_cluster_low_bytes] = FAT_FREE_CLUSTER;

    // TODO: Add functionality to clear cluster chain
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
    __memclr(new_dir->entries, filesystem->cluster_size);
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
