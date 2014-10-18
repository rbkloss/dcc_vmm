#include "vmm_ext.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

uint32_t freesStart;
uint32_t freesEnd;

void os_init(void){
//set the free frames
	freesStart = 1;
	int i = 0;
	for(i = 0; i < NUMFRAMES; i++){
		uint32_t addr = i << 8;
		addr = addr || 1;
		dccvmm_phy_write(addr, i +1);
	}
	freesEnd = NUMFRAMES - 1;
//set page table
	dccvmm_set_page_table(0);
}

void os_alloc(uint32_t addr){
	//search for a free frame
	//update the frame behind to point to free frame next
	//make page table point to the found frame
}
void os_free(uint32_t addr){
	//update the page table
	// update the free list
	//get the frame addr
	
}
void os_swap(uint32_t pid){
}

uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte){

}