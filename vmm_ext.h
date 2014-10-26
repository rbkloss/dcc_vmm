#ifndef __VMM_EXT_H__
#define __VMM_EXT_H__

#include "vmm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define TRUE 1
#define FALSE 0

#define FREEFLAG 0x40000000
#define NUMPAGES 256

uint32_t freesStart_;
uint32_t procTable_;

int outOfMemory_;

unsigned currentPID_;

uint32_t getFreeFrame();

//parte 2
void os_init(void);


//parte 3
void os_alloc(uint32_t addr);
void os_free(uint32_t addr);
uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte);

//parte 4
void os_swap(uint32_t pid);
#endif