/**
** @file
** file:   filesystem.h
**
** @author CSCI-452 class of 20215
**
** author: Eric Chen
**
** description: Header file for filesystem implementation
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

// Possible values for file attributes
#define DIR_ENTRY_READ_ONLY 0x01
#define DIR_ENTRY_HIDDEN 0x02
#define DIR_ENTRY_SYSTEM 0x04
#define DIR_ENTRY_VOLUME_ID 0x08
#define DIR_ENTRY_DIRECTORY 0x10
#define DIR_ENTRY_ARCHIVE 0x20

/*
** Types
*/

/*
** BIOS Parameter Block that contains information about the boot record
**
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
    char reserved_0[12];
    uint8_t drive_num;
    uint8_t windows_flags; //Only used for flags in Windows NT, reserved otherwise
    uint8_t signature;
    uint32_t volume_id;
    char volume_label[12];
    char system_id[9];
} bpb_t;

/*
** FAT Filesystem that contains the boot record, the file allocation table (FAT)
** and information about the sectors, clusters, and where they begin.
**
*/
typedef struct FAT32_struct {
    bpb_t bios_block;
    uint32_t *FAT;
    uint32_t partition_begin_sector;
    uint32_t FAT_begin_sector;
    uint32_t cluster_begin_sector;
    uint32_t cluster_size;
    uint32_t current_cluster_pos;
} f32_t;

/*
** Basic structure for a directory entry (files)
**
*/
typedef struct director_entry {
    char name[8];
    char extension[3];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t first_cluster_high_bytes;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_low_bytes;
    uint32_t file_size;
} dir_entry_t;

/*
** Structure used for a directory containing multiple directory entries
**
** *Note: May not be needed*
*/
typedef struct directory {
    uint32_t cluster_num;
    dir_entry_t *entries;
    uint32_t num_entries;
} directory_t;

/*
** Globals
*/

/*
** Prototypes
*/

f32_t *make_Filesystem();

void end_Filesystem(f32_t *filesystem);

dir_entry_t *create_file(char* new_name, char* type, uint32_t size);

//void file_open(dir_entry_t* this_file, char* mode);

//void file_close(dir_entry_t* this_file);

void file_read(f32_t *filesystem, dir_entry_t* this_file, uint32_t cluster);

void file_write(f32_t *filesystem, dir_entry_t* this_file, directory_t *this_dir);

void delete_file(f32_t *filesystem, dir_entry_t* del_file, directory_t *this_dir);

directory_t *create_dir(f32_t *filesystem, uint32_t cluster);

void rm_dir();

#endif
/* SP_ASM_SRC */

#endif
