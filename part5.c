#include "vmm_ext.h"

void dumpProcess(int pid) {
    //TODO
    uint32_t dirSector = dccvmm_phy_read(procTable_ << 8 | pid);
    dumpPageDir(pid);
}

void dumpPageDir(int pid) {
    //TODO
}

void dumpPageTable(uint32_t dirAddr, uint32_t dirSector) {
    uint32_t procPT = dccvmm_phy_read(dirAddr);
    if (procPT & PTE_VALID != PTE_VALID) {
        printf("Not a Valid Page to Dump!\n");
        return;
    }

    dccvmm_load_frame(dirSector, SWAP_FRAME);
    uint32_t ptSector = dccvmm_phy_read(SWAP_FRAME << 8 | PTE1OFF(dirAddr));
    if (ptSector & PTE_VALID != PTE_VALID) {
        //pt does not exists on disk
        //we have to create it
        ptSector = getFreeSector();
        dccvmm_phy_write(SWAP_FRAME << 8 | PTE1OFF(dirAddr), ptSector | PTE_VALID);        
        dccvmm_dump_frame(SWAP_FRAME, dirSector);
        dccvmm_zero(SWAP_FRAME);
        dccvmm_dump_frame(SWAP_FRAME, ptSector);
    }

    int i = 0;
    for (i = 0; i < NUMWORDS; i++) {
        dumpPTE(procPT << 8 | i, ptSector);
        dccvmm_phy_write(procPT << 8 | i, dccvmm_phy_read(procPT << 8 | i) & PTE_VALID);
    }
}

/**
 * stores a frame in the disk
 * @param ptAddr
 * @param ptSector
 */
void dumpPTE(uint32_t ptAddr, uint32_t ptSector) {
    uint32_t pte = dccvmm_phy_read(ptAddr);
    dccvmm_phy_write(ptAddr, PTE_VALID | PTE_RW);

    dccvmm_load_frame(ptSector, SWAP_FRAME);
    uint32_t diskPTE = dccvmm_phy_read(SWAP_FRAME << 8 | PTE2OFF(ptAddr));
    if (diskPTE & PTE_VALID != PTE_VALID) {
        diskPTE = getFreeSector();
        dccvmm_phy_write(SWAP_FRAME << 8 | PTE2OFF(ptAddr), diskPTE | PTE_VALID);
        dccvmm_dump_frame(SWAP_FRAME, diskPTE);
    }
    dccvmm_dump_frame(pte, diskPTE);
}

//void getFreeSector();