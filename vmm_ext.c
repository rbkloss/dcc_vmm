#include "vmm_ext.h"

uint32_t getFreeFrame() {
    uint32_t freeFrame;
    if (outOfMemory_) {
        while (TRUE) {
            int count = 0;
            uint32_t victimPID, dir, dirFrame, pt, ptFrame, pte, pteFrame;
            victimPID = rand() % 256;
            dir = dccvmm_phy_read(procTable_ << 8 | victimPID);
            while (!(dir & PTE_INMEM) && count < 256) {
                victimPID = (victimPID + 1) % 256;
                dir = dccvmm_phy_read(procTable_ << 8 | victimPID);
                count++;
            }
            if (count == 256)continue;
            else count = 0;
            dirFrame = PTEFRAME(dir);

            uint32_t ptRow = rand() % 256;
            pt = dccvmm_phy_read(dirFrame << 8 | ptRow);
            while (!(pt & PTE_INMEM) && count < 256) {
                ptRow = (ptRow + 1) % 256;
                pt = dccvmm_phy_read(dirFrame << 8 | ptRow);
                count++;
            }
            if (count == 256)continue;
            else count = 0;
            ptFrame = PTEFRAME(pt);

            uint32_t row = rand() % 256;
            pte = dccvmm_phy_read(ptFrame << 8 | row);
            pteFrame = PTEFRAME(pte);
            while (!(pte & PTE_INMEM) && count < 256) {
                row = (row + 1) % 256;
                pte = dccvmm_phy_read(ptFrame << 8 | row);
                pteFrame = PTEFRAME(pte);
                count++;
            }
            if (count == 256)continue;
            else count = 0;
            dumpPTE(row << 8, ptFrame);
            int i = 0, empty = TRUE;
            for (i = 0; i < NUMWORDS; i++) {
                uint32_t entry = dccvmm_phy_read(ptFrame << 8 | i);
                if ((entry & PTE_INMEM)) {
                    empty = FALSE;
                }
            }
            if (empty) dumpPageTable(ptRow << 8, dirFrame, &pt);
            freeFrame = pteFrame;
            break;
        }
    } else {
        freeFrame = freesStart_;
        freesStart_ = dccvmm_phy_read(freeFrame << 8);
        if (freesStart_ == 0) {
            //this is the last free frame
            //out of memory
            outOfMemory_ = TRUE;
        }
    }
    dccvmm_zero(freeFrame);
    return PTEFRAME(freeFrame);
}

void os_alloc(uint32_t addr) {
    //search for a free frame
    if (PAGEOFFSET(addr)) {
        // offset must be *00;
        fprintf(stderr, "Invalid Alloc on address :[%x]\n", addr);
        return;
    }
    uint32_t dir, dirFrame, pt, ptFrame;
    //make page table point to the found frame
    dir = dccvmm_phy_read(procTable_ << 8 | currentPID_);
    dirFrame = PTEFRAME(dir);
    pt = dccvmm_phy_read(
            dirFrame << 8 | PTE1OFF(addr));
    ptFrame = PTEFRAME(pt);
    if ((pt & PTE_VALID) != PTE_VALID) {
        //pointed frame is not a valid frame 
        ptFrame = getFreeFrame();
        dccvmm_phy_write(dirFrame << 8 | PTE1OFF(addr),
                ptFrame | PTE_VALID | PTE_INMEM | PTE_RW);
    }

    if (dccvmm_phy_read(ptFrame << 8 | PTE2OFF(addr)) & PTE_VALID) {
        fprintf(stderr, "\tDouble Alloc Detected\n");
        return;
    }
    uint32_t freeFrame = getFreeFrame();
    // frame is not avaliable anymore;    
    //write on the page table the allocated Frame
    dccvmm_phy_write(ptFrame << 8 | PTE2OFF(addr),
            freeFrame | PTE_VALID | PTE_INMEM | PTE_RW);
}

void os_free(uint32_t addr) {
    //find the phy addres relative to the virtual address
    uint32_t dir, dirFrame, pt, ptFrame, pte, pteFrame;
    dir = (dccvmm_phy_read(procTable_ << 8 | currentPID_));
    dirFrame = PTEFRAME(dir);
    pt = dccvmm_phy_read(dirFrame << 8 | PTE1OFF(addr));
    ptFrame = PTEFRAME(pt);
    pte = dccvmm_phy_read(ptFrame << 8 | PTE2OFF(addr));
    pteFrame = PTEFRAME(pte);
    //update the page table
    dccvmm_phy_write(ptFrame << 8 | PTE2OFF(addr), 0);
    //update the free list:
    dccvmm_phy_write(pteFrame << 8, freesStart_);
    //	make the freed frame the freeStart and make it point to the old freeStart
    freesStart_ = pteFrame;
    if (outOfMemory_)outOfMemory_ = FALSE;

    int emptyFlag = TRUE;
    int i = 0;
    for (i = 0; i < NUMWORDS; i++) {
        uint32_t pte = dccvmm_phy_read(ptFrame << 8 | i);
        if ((pte & PTE_INMEM) == PTE_INMEM) {
            //page Table has valid pages
            emptyFlag = FALSE;
        }
    }
    if (emptyFlag) {
        ptFrame = PTEFRAME(ptFrame);
        dumpPageTable(addr, dirFrame, &pt);
    }
}

uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte) {
    if ((pte & PTE_VALID) != PTE_VALID) {
        fprintf(stderr,
                "\t!!Faulty memory access @[0x%x] pte[0x%x]\n", address, pte);
        return VM_ABORT;
    } else if ((pte & PTE_INMEM) != PTE_INMEM) {
        uint32_t dir, pt;

        loadPageDir(currentPID_, &dir);
        loadPageTable(address, PTEFRAME(dir), &pt);
        loadPTE(address, PTEFRAME(pt));
    }
    return 0;
}

