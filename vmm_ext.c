#include "vmm_ext.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define NUMPAGES 256

uint32_t freesStart_;
uint32_t freesEnd_;
uint32_t procTable_;

uint32_t getFreeFrame(){
	uint32_t freeFrame = freeStart_;
	freeStart = dccvmm_phy_read(freeFrame <<8 || 0);
	return freeFrame;
}

void os_init(void){
//set the free frames
	freesStart_ = 2;
	procTable = 1;
	int i = 0;
	for(i = 2; i < NUMFRAMES; i++){
		uint32_t addr = i << 8;
		addr = addr || 1;
		dccvmm_phy_write(addr, i +1);
	}
	freesEnd_ = NUMFRAMES - 1;
//set page table
	dccvmm_set_page_table(0);
}

void os_alloc(uint32_t addr){
	//search for a free frame
	//update the frame behind to point to next free frame
	//make page table point to the found frame
}
void os_free(uint32_t addr){
	//update the page table
	//update the free list:
	//	make the freed frame the freeStart and make it point to the old freeStart
	//get the frame addr
	
}
void os_swap(uint32_t pid){
	//look for the frame of the process page table
	// if not on the memory, load from disk
	//call function to set the pagetable
	uint32_t procPT = dccvmm_phy_read(procTable_ << 8 | pid);
	if(procPT & PTE_INMEM){
		dccvmm_set_page_table(dccvmm_phy_read());
	}else{
		if(procPT & PTE_VALID){
		//process exists but is in the disk
			//TODO
		}else{
			procPT = getFreeFrame();
			dccvmm_phy_write(procTable_ << 8 | pid, procPT);
		}
	}
}

uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte){

}