#include "vmm_ext.h"

uint32_t getFreeFrame() {
    if (outOfMemory_) {
        //TODO move victim frame to disk, free it and return.
    }
    uint32_t freeFrame = freesStart_;
    freesStart_ = dccvmm_phy_read(freeFrame << 8);
    if (freesStart_ == 0) {
        //this is the last free frame
        //out of memory
        outOfMemory_ = TRUE;
    }
    printf("||Found Free Frame @(%x)||\n", freeFrame);
    return freeFrame;
}

void os_alloc(uint32_t addr) {
    //search for a free frame
    printf("\tAllocating memory ...\n");
    if (PAGEOFFSET(addr)) {
        fprintf(stderr, "Invalid Alloc on address :[%x]\n", addr);
        return;
    }
    //make page table point to the found frame
    uint32_t pageDir = dccvmm_phy_read(procTable_ << 8 | currentPID_);
    uint32_t frameOfPageTable = dccvmm_phy_read(
            PTEFRAME(pageDir) << 8 | PTE1OFF(addr));
    if ((frameOfPageTable & PTE_VALID) != PTE_VALID) {
        //pointed frame is not a valid frame 
        frameOfPageTable = getFreeFrame();
        dccvmm_phy_write(PTEFRAME(pageDir) << 8 | PTE1OFF(addr),
                frameOfPageTable | PTE_VALID | PTE_INMEM | PTE_RW);
    }
    if (dccvmm_phy_read(PTEFRAME(frameOfPageTable) << 8 | PTE2OFF(addr)) & PTE_VALID) {
        fprintf(stderr, "\tDouble Alloc Detected\n");
        return;
    }
    uint32_t freeFrame = getFreeFrame();    
    freeFrame = freeFrame & (~FREEFLAG);
    //write on the page table the allocated Frame
    dccvmm_phy_write(PTEFRAME(frameOfPageTable) << 8 | PTE2OFF(addr),
            freeFrame | PTE_VALID | PTE_INMEM | PTE_RW);

    printf("\t||Allocated [%x] @:[%x]\n", addr, freeFrame);
}

void os_free(uint32_t addr) {//TODO check for errors and exceptions
    //find the phy addres relative to the virtual address
    printf("Freeing memory ...\n");
    uint32_t pdAddr = dccvmm_phy_read(procTable_ << 8 | currentPID_);
    printf("pageDir @[%x]\n", pdAddr);
    uint32_t ptFrame = dccvmm_phy_read(pdAddr << 8 | PTE1OFF(addr));
    printf("pageTable @[%x]\n", ptFrame);
    uint32_t phyAddr = dccvmm_phy_read(PTEFRAME(ptFrame << 8) | PTE2OFF(addr));
    printf("freedFrame @[%x]\n", phyAddr);
    //update the page table
    dccvmm_phy_write(PTEFRAME(ptFrame << 8) | PTE2OFF(addr), 0);
    //update the free list:
    dccvmm_phy_write(PTEFRAME(phyAddr << 8), freesStart_ | FREEFLAG);
    //	make the freed frame the freeStart and make it point to the old freeStart
    freesStart_ = phyAddr;

    printf("\t||Freed phy[%x] virt[%x]\n", phyAddr, addr);
}

uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte) {
    if ((pte & PTE_VALID) != PTE_VALID) {
        fprintf(stderr,
                "\t!!Faulty memmory access @[%x] pte[%x]\n", address, pte);
        return VM_ABORT;
    } else if ((pte & PTE_INMEM) == PTE_INMEM) {
        //TODO pte on disk

    }
}