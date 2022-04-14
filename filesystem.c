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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <stdint.h>
#include <math.h>

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

}

f32_t *make_Filesystem(char *FATsystem){
    f32_t *filesystem = kmalloc(sizeof(f32_t));

    //Get information about bpb
    read_bpb(filesystem, filesystem->bios_block);

    filesystem->partition_begin_sector = 0;
    filesystem->FAT_begin_sector = filesystem->partition_begin_sector * filesystem->bios_block.reserved_sectors;
    filesystem->cluster_begin_sector = filesystem->FAT_begin_sector + (filesystem->bios_block.num_FAT * filesystem->bios_block.sectors_per_FAT32);
    filesystem->cluster_size = 512 * filesystem->bios_block.sectors_per_cluster;
    filesystem->current_cluster_pos = 0;

    //Set up the File Allocation Table
    uint32_t FAT_size = 512 * filesystem->bios_block.sectors_per_FAT32;
    filesystem->FAT = kmalloc(FAT_size);
    //Set up sectors

    return filesystem;
}

void end_Filesystem(f32_t *filesystem){
    kfree(filesystem->FAT);
    kfree(filesystem);
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
dir_entry_t create_file(char* new_name, char* type, uint32_t size){
    if(strlen(new_name) > MAX_FILENAME){
        return -1;
    }
    if(strlen(type) > MAX_FILETYPE){
        return -1;
    }

    // TODO Check if filename exists in FAT
    
    dir_entry_t* new_file = malloc(sizeof(dir_entry_t));
    new_file->name = new_name;
    new_file->extension = type;
    new_file->file_size = size;

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
void file_read(dir_entry_t new_file){

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
void file_write(f32_t *filesystem, dir_entry_t new_file){

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
void delete_file(dir_entry_t del_file){

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
    new_dir->name = dir_name;

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
        kfree(dir->entries[i].name);
    }
    kfree(dir->entries);
}
