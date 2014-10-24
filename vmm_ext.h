#ifndef __VMM_EXT_H__
#define __VMM_EXT_H__

#include <inttypes.h>
#include "vmm.h"

uint32_t start_free_p = 0;
typedef struct Process Process;
void os_init(void);

void os_alloc(uint32_t addr);
void os_free(uint32_t addr);
void os_swap(uint32_t pid);
uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte);
#endif
