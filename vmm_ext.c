#include "vmm_ext.h"

uint32_t getFreeFrame() {
    uint32_t freeFrame;
    if (outOfMemory_) {
        //TODO move victim frame to disk, free it and return.
        printf("Memory is FULL looking for a victim\n\n");
        uint32_t victimPID = rand() % 256;
        uint32_t victimDir = dccvmm_phy_read(procTable_ << 8 | victimPID);
        while (victimDir & PTE_INMEM != PTE_INMEM) {
            victimPID = (victimPID + 1) % 256;
            victimDir = dccvmm_phy_read(procTable_ << 8 | victimPID);
        }
        if (victimPID == currentPID_)victimPID = (victimPID + 1) % 256;

        uint32_t row = rand() % 256;
        uint32_t victimPT = dccvmm_phy_read(victimDir << 8 | row);
        while (victimPT & PTE_INMEM != PTE_INMEM) {
            row = (row + 1) % 256;
            victimPT = dccvmm_phy_read(victimDir << 8 | row);
        }
        freeFrame = dccvmm_phy_read(victimPT << 8 | row);
        row = rand() % 256;
        dumpPTE(row << 8, victimPT);
    } else {
        freeFrame = freesStart_;
        freesStart_ = dccvmm_phy_read(freeFrame << 8);
        if (freesStart_ == 0) {
            //this is the last free frame
            //out of memory
            outOfMemory_ = TRUE;
            printf("LAST FRAME ALLOCD\n");
        }
    }
    printf("||Found Free Frame @(%x)||\n", freeFrame);
    dccvmm_zero(freeFrame);
    return freeFrame;
}

void os_alloc(uint32_t addr) {
    //search for a free frame
    printf("\tAllocating memory ...\n");
    if (PAGEOFFSET(addr)) {
        // offset must be *00;
        fprintf(stderr, "Invalid Alloc on address :[%x]\n", addr);
        return;
    }
    //make page table point to the found frame
    uint32_t pageDir = dccvmm_phy_read(procTable_ << 8 | currentPID_);
    uint32_t pt= dccvmm_phy_read(
            PTEFRAME(pageDir) << 8 | PTE1OFF(addr));
    if ((pt & PTE_VALID) != PTE_VALID) {
        //pointed frame is not a valid frame 
        pt = getFreeFrame();
        dccvmm_phy_write(PTEFRAME(pageDir) << 8 | PTE1OFF(addr),
                pt | PTE_VALID | PTE_INMEM | PTE_RW);
    }
    if (dccvmm_phy_read(PTEFRAME(pt) << 8 | PTE2OFF(addr)) & PTE_VALID) {
        fprintf(stderr, "\tDouble Alloc Detected\n");
        return;
    }
    uint32_t freeFrame = getFreeFrame();
    // frame is not avaliable anymore;
    freeFrame = freeFrame & (~FREEFLAG);
    //write on the page table the allocated Frame
    dccvmm_phy_write(PTEFRAME(pt) << 8 | PTE2OFF(addr),
            freeFrame | PTE_VALID | PTE_INMEM | PTE_RW);

    printf("\t||Allocated [%x] @:[%x]\n", addr, freeFrame);
}

void os_free(uint32_t addr) {
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
    if (outOfMemory_)outOfMemory_ = FALSE;
    printf("\t||Freed phy[%x] virt[%x]\n", phyAddr, addr);

    int emptyFlag = TRUE;
    int i = 0;
    for (i = 0; i < NUMWORDS; i++) {
        uint32_t pte = dccvmm_phy_read(PTEFRAME(ptFrame << 8) | i);
        if (pte & PTE_INMEM == PTE_INMEM) {
            //page Table has valid pages
            emptyFlag = FALSE;
        }
    }
    if (emptyFlag) {
        dumpPageTable(addr, pdAddr, &ptFrame);
    }
}

uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte) {
    if ((pte & PTE_VALID) != PTE_VALID) {
        fprintf(stderr,
                "\t!!Faulty memory access @[%x] pte[%x]\n", address, pte);
        return VM_ABORT;
    } else if ((pte & PTE_INMEM) == PTE_INMEM) {
        uint32_t dir, pt;

        loadPageDir(currentPID_, &dir);
        loadPageTable(address, dir, &pt);
        loadPTE(address, pt);
    }
}

