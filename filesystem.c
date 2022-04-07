/**
** @file
** file: filesystem.c
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
file_t create_file(char* name, char* type, uint32_t size){
    if(strlen(name) > MAX_FILENAME){
        return -1;
    }
    if(strlen(type) > MAX_FILETYPE){
        return -1;
    }

    // TODO Check if filename exists in FAT
    
    file_t* new_file = malloc(sizeof(file_t));
    new_file->filename = name;
    new_file->filetype = type;

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
void file_open(file_t new_file, char* mode){

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
void file_close(file_t new_file){

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
void delete_file(file_t del_file){

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
void create_dir(){

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
void rm_dir(){

}
