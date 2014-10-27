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
#define PTE_SECTOR(addr) (addr & 0xfffff)
#define NUMWORDS 256
#define SWAP_FRAME 13

uint32_t freesStart_;
uint32_t procTable_;
uint32_t diskProcTable_;
int outOfMemory_;

unsigned currentPID_;

uint32_t getFreeFrame();
uint32_t getFreeSector();

//parte 2
void os_init(void);


//parte 3
void os_alloc(uint32_t addr);
void os_free(uint32_t addr);
uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte);

//parte 4
void os_swap(uint32_t pid);

//parte 5

void dumpProcess(int pid);
void dumpPageDir(int pid, uint32_t* dir);
void dumpPageTable(uint32_t address, uint32_t dir, uint32_t *pt);
void dumpPTE(uint32_t address, uint32_t pt);


//TODO
void loadProcess(int pid);
void loadPageDir(int pid, uint32_t* dir);
void loadPageTable(uint32_t address, uint32_t dir, uint32_t *pt);
void loadPTE(uint32_t address, uint32_t pt);

void copyFrames(uint32_t source, uint32_t dest);

uint32_t getFreeSector();
#endif