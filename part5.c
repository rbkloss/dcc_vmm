#include "vmm_ext.h"

void dumpProcess(int pid) {
    uint32_t dir;
    dumpPageDir(pid, &dir);
}

void dumpPageDir(int pid, uint32_t* dir) {
    uint32_t diskDIR;
    *dir = dccvmm_phy_read(procTable_ << 8 | pid);
    if ((*dir) & PTE_INMEM != PTE_INMEM) {
        fprintf(stderr, "Target frame for dump is not in mem\n");
    } else if ((*dir) & PTE_VALID == PTE_VALID) {
        int i = 0;
        for (i = 0; i < NUMWORDS; i++) {
            uint32_t pt;
            dumpPageTable(i << 8, *dir, &pt);
        }
        diskDIR = PTE_SECTOR(getFreeSector());
        dccvmm_dump_frame(diskDIR, *dir);
        dccvmm_phy_write(procTable_ << 8 | pid,
                diskDIR | PTE_VALID | PTE_RW);
    }
}

void dumpPageTable(uint32_t address, uint32_t dir, uint32_t *pt) {
    uint32_t diskPT;
    *pt = dccvmm_phy_read(dir << 8 | PTE2OFF(address));
    if ((*pt) & PTE_INMEM != PTE_INMEM) {
        fprintf(stderr, "Target frame for dump is not in mem\n");
    } else if ((*pt) & PTE_VALID == PTE_VALID) {
        int i = 0;
        for (i = 0; i < NUMWORDS; i++) {
            dumpPTE(i << 8, *pt);
        }
        diskPT = PTE_SECTOR(getFreeSector());
        dccvmm_dump_frame(diskPT, *pt);
        dccvmm_phy_write(dir << 8 | PTE1OFF(address),
                diskPT | PTE_VALID | PTE_RW);
    }
}

void dumpPTE(uint32_t address, uint32_t pt) {
    uint32_t diskPTE;
    uint32_t pte = dccvmm_phy_read(pt << 8 | PTE2OFF(address));
    if (pte & PTE_INMEM != PTE_INMEM) {
        fprintf(stderr, "Target frame for dump is not in mem\n");
    } else if (pte & PTE_VALID == PTE_VALID) {
        diskPTE = PTE_SECTOR(getFreeSector());
        dccvmm_dump_frame(diskPTE, pte);
        dccvmm_phy_write(pt << 8 | PTE2OFF(address),
                diskPTE | PTE_VALID | PTE_RW);
    }
}


//TODO void getFreeSector();

void copyFrames(uint32_t source, uint32_t dest) {
    int i;
    for (i = 0; i < NUMWORDS; i++) {
        dccvmm_phy_write(dest << 8 | i, dccvmm_phy_read(source << 8 | i));
    }
}

void loadProcess(int pid) {
    uint32_t dir;
    loadPageDir(pid, &dir);
}

void loadPageDir(int pid, uint32_t *dir) {
    uint32_t diskDir;
    *dir = dccvmm_phy_read(procTable_ << 8 | currentPID_);
    if (*dir & PTE_INMEM != PTE_INMEM) {
        diskDir = PTE_SECTOR(*dir);
        *dir = getFreeFrame();
        dccvmm_load_frame(diskDir, *dir);
        dccvmm_phy_write(procTable_ << 8 | currentPID_,
                (*dir) | PTE_VALID | PTE_RW | PTE_INMEM);
    }
}

void loadPageTable(uint32_t address, uint32_t dir, uint32_t *pt) {
    uint32_t diskPT;
    *pt = dccvmm_phy_read(dir << 8 | PTE1OFF(address));
    if (*pt & PTE_INMEM != PTE_INMEM) {
        diskPT = PTE_SECTOR(dir);
        *pt = getFreeFrame();
        dccvmm_load_frame(diskPT, *pt);
        dccvmm_phy_write(dir << 8 | PTE1OFF(address),
                (*pt) | PTE_VALID | PTE_RW | PTE_INMEM);
    }
}

void loadPTE(uint32_t address, uint32_t pt) {
    uint32_t diskPTE;
    uint32_t mpte = dccvmm_phy_read(pt << 8 | PTE2OFF(address));
    if (mpte & PTE_INMEM != PTE_INMEM) {
        diskPTE = PTE_SECTOR(mpte);
        mpte = getFreeFrame();
        dccvmm_load_frame(diskPTE, mpte);
        dccvmm_phy_write(pt << 8 | PTE2OFF(address),
                mpte | PTE_VALID | PTE_RW | PTE_INMEM);
    }
}