#ifndef _disk_h
#define _disk_h

#include "vmm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

void disk_init ();
uint32_t getFreeSector ();
void setSectorUsed (uint32_t sectorID);
uint32_t checkBits (uint32_t data);
#endif