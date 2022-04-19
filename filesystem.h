/**
** @file
** file:   filesystem.h
**
** @author CSCI-452 class of 20215
**
** author: Eric Chen
**
** description:
*/

#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

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

#define MAX_FILENAME 8
#define MAX_FILETYPE 3
#define MAXBLOCKS 1024
#define BLOCKSIZE 1024

/*
** Types
*/

typedef struct bios_param_block{
    char jmp[3];
    char oem[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_FAT;
    uint16_t num_root_dir;
    uint16_t total_sectors; //If 0 then > 65535 so use large_sector_count
    uint8_t media_descriptor_type;
    uint16_t num_sectors_per_FAT; //Only used if this was FAT12/FAT16
    uint16_t num_sectors_per_track;
    uint16_t num_heads_media;
    uint32_t num_hidden_sectors;
    uint32_t large_sector_count; //Used if more than 65535 sectors in the volume

    //Extended Boot Record
    uint32_t sectors_per_FAT32;
    uint16_t flags;
    uint16_t FAT_version_num;
    uint32_t root_dir_cluster_num;
    uint16_t sector_num_FSInfo;
    uint16_t sector_num_backup;
    uint8_t drive_num;
    uint8_t windows_flags;
    uint8_t signature;
    uint32_t volume_id;
    char volume_label[12];
    char system_id[9];
} bpb_t;


typedef struct FAT32_struct {
    uint32_t *FAT;
    bpb_t bios_block;
    uint32_t partition_begin_sector;
    uint32_t FAT_begin_sector;
    uint32_t cluster_begin_sector;
    uint32_t cluster_size;
    uint32_t current_cluster_pos;
} f32_t;


typedef struct director_entry {
    char name[8];
    char extension[3];
    char attributes[9];
    uint32_t first_cluster;
    uint32_t file_size;
} dir_entry_t;

typedef struct directory {
    uint32_t cluster;
    dir_entry_t *entries;
    uint32_t num_entries;
} directory_t;

/*
** Globals
*/

/*
** Prototypes
*/

f32_t *make_Filesystem(char *FATsystem);
void end_Filesystem(f32_t *filesystem);

dir_entry_t *create_file(char* new_name, char* type, uint32_t size);

void delete_file(f32_t *filesystem, dir_entry_t* del_file);

//void file_open(dir_entry_t* this_file, char* mode);

//void file_close(dir_entry_t* this_file);

void file_read(f32_t *filesystem, dir_entry_t* this_file);

void file_write(f32_t *filesystem, dir_entry_t* this_file);

void create_dir();

void rm_dir();

#endif
/* SP_ASM_SRC */

#endif
