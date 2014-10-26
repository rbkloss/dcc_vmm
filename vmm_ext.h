#ifndef __VMM_EXT_H__
#define __VMM_EXT_H__

#include "vmm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define NUMPAGES 256

uint32_t freesStart_;
uint32_t freesEnd_;
uint32_t procTable_;

unsigned currentPID_;

uint32_t getFreeFrame();

//parte 2
void os_init(void);


//parte 3
void os_alloc(uint32_t addr);
void os_free(uint32_t addr);
void os_swap(uint32_t pid);

//parte 4
uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte);
#endif