#include "vmm_ext.h"

uint32_t getFreeFrame() {
    uint32_t freeFrame = freesStart_;
    freesStart_ = dccvmm_phy_read(freeFrame << 8);
    printf("||Found Free Frame @(%x)||\n", freeFrame);
    return freeFrame;
}

void os_alloc(uint32_t addr) {//TODO check for errors and exceptions
    //search for a free frame
    printf("\tAllocating memory ...\n");
    if (PAGEOFFSET(addr)) {
        printf("Invalid Alloc on address :[%x]\n", addr);
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
    uint32_t freeFrame = getFreeFrame();
    //write on the page table the allocated Frame
    dccvmm_phy_write(PTEFRAME(frameOfPageTable) << 8 | PTE2OFF(addr), freeFrame | PTE_VALID | PTE_INMEM | PTE_RW);

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
    dccvmm_phy_write(PTEFRAME(phyAddr << 8), freesStart_);
    //	make the freed frame the freeStart and make it point to the old freeStart
    freesStart_ = phyAddr;

    printf("\t||Freed phy[%x] virt[%x]\n", phyAddr, addr);
}

uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte) {
    if ((pte & PTE_VALID) != PTE_VALID) {
        printf("Faulty memmory access @[%x]\n", address);
        return VM_ABORT;
    }

    // TODO: check INMEM flag - part 5
}