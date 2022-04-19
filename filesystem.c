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

uint32_t FAT_FREE_CLUSTER = 0x00000000;
uint32_t FAT_EOC = 0x0FFFFFF8;
uint32_t current_cluster = 0;


/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

void read_bpb(f32_t *filesystem, bpb_t *bios_block){
    uint8_t sector0[512];
    //TODO Find bootsector
    // ata_device_t dev = {}
    //read_sectors_ATA_PIO(0, sector0, );

    bios_block->bytes_per_sector;
    bios_block->sectors_per_cluster;
    bios_block->reserved_sectors;
    bios_block->num_FAT;
    bios_block->num_root_dir;
    bios_block->total_sectors; 
    bios_block->media_descriptor_type;
    bios_block->num_sectors_per_FAT; 
    bios_block->num_sectors_per_track;
    bios_block->num_heads_media;
    bios_block->num_hidden_sectors;
    bios_block->large_sector_count; 

    //Extended Boot Record
    bios_block->sectors_per_FAT32;
    bios_block->flags;
    bios_block->FAT_version_num;
    bios_block->root_dir_cluster_num;
    bios_block->sector_num_FSInfo;
    bios_block->sector_num_backup;
    bios_block->drive_num;
    bios_block->windows_flags;
    bios_block->signature;
    bios_block->volume_id;

}

f32_t *make_Filesystem(char *FATsystem){
    f32_t *filesystem;
    __memclr(filesystem, sizeof(f32_t));

    //Get information about bpb
    read_bpb(filesystem, &filesystem->bios_block);

    filesystem->partition_begin_sector = 0;
    filesystem->FAT_begin_sector = filesystem->partition_begin_sector * filesystem->bios_block.reserved_sectors;
    filesystem->cluster_begin_sector = filesystem->FAT_begin_sector + (filesystem->bios_block.num_FAT * filesystem->bios_block.sectors_per_FAT32);
    filesystem->cluster_size = 512 * filesystem->bios_block.sectors_per_cluster;
    filesystem->current_cluster_pos = 0;

    //Set up the File Allocation Table
    uint32_t FAT_size = 512 * filesystem->bios_block.sectors_per_FAT32;
    __memclr(filesystem->FAT, FAT_size);
    
    //Set up sectors
    for(uint32_t sector_count = 0; sector_count < filesystem->bios_block.sectors_per_FAT32; sector_count++){
        uint32_t this_sector[512];
        // ata_device_t dev = {}
        // read_sectors_ATA_PIO(filesystem->FAT_begin_sector + sector_count, this_sector, );


    }


    return filesystem;
}

void end_Filesystem(f32_t *filesystem){
    //free(filesystem->FAT);
    //free(filesystem);
}

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
dir_entry_t *create_file(char* new_name, char* type, uint32_t size){
    if(strlen(new_name) > MAX_FILENAME){
        return -1;
    }
    if(strlen(type) > MAX_FILETYPE){
        return -1;
    }

    // TODO Check if filename exists in FAT
    
    dir_entry_t *new_file;
    __memclr(new_file, sizeof(dir_entry_t));
    __strcpy(new_file->name, new_name);
    __strcpy(new_file->extension, type);
    __strcpy(new_file->file_size, size);

    // TODO Add new file to FAT


    return new_file;
}

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
void file_read(f32_t *filesystem, dir_entry_t *new_file){

}

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
void file_write(f32_t *filesystem, dir_entry_t *new_file){

}

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
void delete_file(f32_t *filesystem, dir_entry_t *del_file){

}

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
void create_dir(f32_t *filesystem, directory_t *this_dir, char *dir_name){
    dir_entry_t *new_dir;
    __strcpy(new_dir->name, dir_name);

    file_write(filesystem, new_dir);
}

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
void rm_dir(f32_t *filesystem, directory_t *dir){
    for(int i = 0; i < dir->num_entries; i++){
        //free(dir->entries[i].name);
    }
    //free(dir->entries);
}
