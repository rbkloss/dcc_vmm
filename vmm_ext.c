#include "vmm_ext.h"

uint32_t getFreeFrame() {
    if (outOfMemory_) {
        //TODO move victim frame to disk, free it and return.
        uint32_t diskDir;
        uint32_t diskPT;

        uint32_t victimPID = rand() % 256;
        uint32_t victimDir;

<<<<<<< HEAD
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
    uint32_t freeFrame = getFreeFrame();
    //make page table point to the found frame
    uint32_t pageDir = dccvmm_phy_read(procTable_ << 8 | currentPID_);    
    uint32_t frameOfPageTable = dccvmm_phy_read(pageDir << 8 | PTE1OFF(addr));
    //write on the page table the allocated Frame
    dccvmm_phy_write(frameOfPageTable<<8 | PTE2OFF(addr), freeFrame | PTE_VALID | PTE_INMEM | PTE_RW);    
}

void os_free(uint32_t addr) {//TODO check for errors and exceptions
    //find the phy addres relative to the virtual address
    uint32_t pdAddr = dccvmm_phy_read(procTable_<< 8 | currentPID_);
    uint32_t ptFrame = dccvmm_phy_read(pdAddr << 8 | PTE1OFF(addr));
    uint32_t phyAddr = dccvmm_phy_read(ptFrame << 8 | PTE2OFF(addr));
    //update the page table
    dccvmm_phy_write(ptFrame << 8 | PTE2OFF(addr), 0);
    //update the free list:
    dccvmm_phy_write(phyAddr << 8, freesStart_);    
    //	make the freed frame the freeStart and make it point to the old freeStart
    freesStart_ = phyAddr;

}

void os_swap(uint32_t pid) {
    //look for the frame of the process page table
    // if not on the memory, load from disk
    //call function to set the pagetable
    uint32_t procPT = dccvmm_phy_read(procTable_ << 8 | pid);
    if (procPT & PTE_INMEM) {
        dccvmm_set_page_table(dccvmm_phy_read(procPT));
    } else {
        if (procPT & PTE_VALID) {
            //process exists but is in the disk
            //TODO
        } else {
            procPT = getFreeFrame();
            dccvmm_phy_write(procTable_ << 8 | pid, procPT);
        }
    }
    currentPID_ = pid;
}

uint32_t os_pagefault(uint32_t address, uint32_t perms, uint32_t pte) {
	if ( (pte & PTE_VALID) != PTE_VALID)
		return VM_ABORT;

	// TODO: check INMEM flag - part 5
}
=======


        while (victimDir & (PTE_VALID | PTE_INMEM) != PTE_VALID | PTE_INMEM) {
            victimPID = (victimPID + 1) % 256;
            victimDir = dccvmm_phy_read(procTable_ << 8 | victimPID);
        }
        diskDir = dccvmm_phy_read(diskProcTable_ << 8 | victimPID);
        if (diskDir & PTE_VALID != PTE_VALID) {
            //not in disk
            //let's put it in the disk
            uint32_t freeSector = getFreeSector();
            dccvmm_load_frame(freeSector, SWAP_FRAME);
            dccvmm_phy_write(diskProcTable_ << 8 | victimPID, freeSector);
            dccvmm_dump_frame(SWAP_FRAME, freeSector);
        }

        uint32_t row = rand() % 256;
        uint32_t victimPT = dccvmm_phy_read(victimDir << 8 | row);
        while (victimPT & (PTE_VALID | PTE_INMEM) != PTE_VALID | PTE_INMEM) {
            row = (row + 1) % 256;
            victimPT = dccvmm_phy_read(victimDir << 8 | row);
        }
        diskPT = dccvmm_phy_read(SWAP_FRAME << 8 | row);
        if (diskPT & PTE_VALID != PTE_VALID) {
            //not in disk
            //let's put it in the disk
            uint32_t freeSector = getFreeSector();
            dccvmm_load_frame(freeSector, SWAP_FRAME);
            dccvmm_phy_write(SWAP_FRAME << 8 | row, freeSector);
            dccvmm_dump_frame(SWAP_FRAME, freeSector);
        }

        row = rand() % 256;
        uint32_t pte = dccvmm_phy_read(victimPT << 8 | row);
        while (pte & (PTE_VALID | PTE_INMEM) != (PTE_VALID | PTE_INMEM)) {
            row = (row + 1) % 256;
            pte = dccvmm_phy_read(victimPT << 8 | row);
        }

        uint32_t freeSector = getFreeSector();
        dccvmm_dump_frame(pte, freeSector);
        dccvmm_phy_write(,);
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

void os_free(uint32_t addr) {
    /*TODO check if after free there is a value in memory
     * if not, dump frame to disk
     */
    
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
>>>>>>> 4e3266549d5782a9f98bcfe04d86264715f44f67
