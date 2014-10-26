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

unsigned currentPID_;

uint32_t getFreeFrame() {
    uint32_t freeFrame = freesStart_;
    freesStart_ = dccvmm_phy_read(freeFrame << 8);
    return freeFrame;
}

void os_init(void) {
    //set the free frames
    freesStart_ = 2;
    procTable_ = 1;
    int i = 0;
    for (i = 2; i < NUMFRAMES; i++) {
        uint32_t addr = i << 8;
        dccvmm_phy_write(addr, i + 1);
    }
    freesEnd_ = NUMFRAMES - 1;
    //set page table
    dccvmm_set_page_table(0);
}

void os_alloc(uint32_t addr) {//TODO check for errors and exceptions
    //search for a free frame
    if (PAGEOFFSET(addr)) {
        printf("Invalid Alloc on address :[%x]\n", addr);
        return;
    }
    uint32_t freeFrame = getFreeFrame();
    //make page table point to the found frame
    uint32_t pageDir = dccvmm_phy_read(procTable_ << 8 | currentPID_);
    uint32_t frameOfPageTable = dccvmm_phy_read(pageDir << 8 | PTE1OFF(addr));
    if ((frameOfPageTable & PTE_VALID) != PTE_VALID) {
        //pointed frame is not a valid frame 
        frameOfPageTable = getFreeFrame();
        dccvmm_phy_write(pageDir << 8 | PTE1OFF(addr),
                frameOfPageTable | PTE_VALID | PTE_INMEM | PTE_RW);
    }

    //write on the page table the allocated Frame
    dccvmm_phy_write(frameOfPageTable << 8 | PTE2OFF(addr), freeFrame | PTE_VALID | PTE_INMEM | PTE_RW);
}

void os_free(uint32_t addr) {//TODO check for errors and exceptions
    //find the phy addres relative to the virtual address
    uint32_t pdAddr = dccvmm_phy_read(procTable_ << 8 | currentPID_);
    printf("pageDir @[%x]\n", pdAddr);
    uint32_t ptFrame = dccvmm_phy_read(pdAddr << 8 | PTE1OFF(addr));
    printf("pageTable @[%x]\n", ptFrame);
    uint32_t phyAddr = dccvmm_phy_read(PTEFRAME(ptFrame << 8) | PTE2OFF(addr));
    printf("freedFrame @[%x]\n", phyAddr);
    //update the page table
    dccvmm_phy_write(PTEFRAME(ptFrame << 8) | PTE2OFF(addr), 0);
    //update the free list:
    dccvmm_phy_write(PTEFRAME(phyAddr << 8), freesStart_);
    //	make the freed frame the freeStart and make it point to the old freeStart
    freesStart_ = phyAddr;

}

void os_swap(uint32_t pid) {
    //look for the frame of the process page table
    // if not on the memory, load from disk
    //call function to set the pagetable
    uint32_t processDirEntries = dccvmm_phy_read(procTable_ << 8 | pid);
    if (processDirEntries & PTE_INMEM) {
        dccvmm_set_page_table(dccvmm_phy_read(processDirEntries));
    } else {
        if (processDirEntries & PTE_VALID) {
            //process exists but is in the disk
            //TODO
        } else {
            processDirEntries = getFreeFrame();
            dccvmm_phy_write(procTable_ << 8 | pid, processDirEntries);
        }
    }
    currentPID_ = pid;
}

uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte) {
    if ((pte & PTE_VALID) != PTE_VALID) {
        printf("Faulty memmory access @[%x]\n", address);
        return VM_ABORT;
    }

    // TODO: check INMEM flag - part 5
}