#include "vmm_ext.h"
#include "disk.h"

void dumpProcess(int pid) {
    uint32_t dir;
    dumpPageDir(pid, &dir);
}

void dumpPageDir(int pid, uint32_t* dir) {
    uint32_t diskDIR;
    *dir = dccvmm_phy_read(procTable_ << 8 | pid);
    if (((*dir) & PTE_INMEM) != PTE_INMEM) {
        //        fprintf(stderr, "Target frame for dump is not in mem %s line %d\n", __FILE__, __LINE__);
    } else if (((*dir) & PTE_VALID) == PTE_VALID) {
        *dir = PTEFRAME(*dir);
        int i = 0;
        for (i = 0; i < NUMWORDS; i++) {
            uint32_t pt;
            dumpPageTable(i << 8, *dir, &pt);
        }
        diskDIR = PTE_SECTOR(getFreeSector());
        dccvmm_dump_frame(*dir, diskDIR);
        dccvmm_phy_write(procTable_ << 8 | pid,
                diskDIR | PTE_VALID | PTE_RW);
    }
}

void dumpPageTable(uint32_t address, uint32_t dir, uint32_t *pt) {
    uint32_t diskPT;
    *pt = dccvmm_phy_read(((dir << 8) | PTE1OFF(address)));
    if (((*pt) & PTE_INMEM) != PTE_INMEM) {
        //        fprintf(stderr, "Target frame for dump is not in mem %s line %d\n", __FILE__, __LINE__);
    } else if ((*pt) & PTE_VALID) {
        *pt = PTEFRAME(*pt);
        int i = 0;
        for (i = 0; i < NUMWORDS; i++) {
            dumpPTE(i << 8, *pt);
        }
        diskPT = PTE_SECTOR(getFreeSector());
        dccvmm_dump_frame(*pt, diskPT);
        dccvmm_phy_write(dir << 8 | PTE1OFF(address),
                diskPT | PTE_VALID | PTE_RW);
    }
}

void dumpPTE(uint32_t address, uint32_t ptFrame) {
    uint32_t diskPTE;
    uint32_t pte = dccvmm_phy_read(ptFrame << 8 | PTE2OFF(address));
    if ((pte & PTE_INMEM) != PTE_INMEM) {
        //        fprintf(stderr, "Target frame for dump is not in mem %s line %d\n", __FILE__, __LINE__);
    } else if ((pte & PTE_VALID) == PTE_VALID) {
        pte = PTEFRAME(pte);
        diskPTE = PTE_SECTOR(getFreeSector());
        dccvmm_dump_frame(pte, diskPTE);
        dccvmm_phy_write(ptFrame << 8 | PTE2OFF(address),
                diskPTE | PTE_VALID | PTE_RW);
    }
}

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
    if ((*dir & PTE_INMEM) != PTE_INMEM) {
        diskDir = PTE_SECTOR(*dir);
        assert(diskDir > 127);
        *dir = PTEFRAME(getFreeFrame());
        dccvmm_load_frame(diskDir, *dir);
        dccvmm_phy_write(procTable_ << 8 | currentPID_,
                (*dir) | PTE_VALID | PTE_RW | PTE_INMEM);
        unsetSectorUsed(diskDir);
    }
}

void loadPageTable(uint32_t address, uint32_t dirFrame, uint32_t *pt) {
    uint32_t diskPT;
    *pt = dccvmm_phy_read(dirFrame << 8 | PTE1OFF(address));
    if (((*pt) & PTE_INMEM) != PTE_INMEM) {
        diskPT = PTE_SECTOR(*pt);
        assert(diskPT > 127);
        *pt = getFreeFrame();
        dccvmm_load_frame(diskPT, *pt);
        dccvmm_phy_write(dirFrame << 8 | PTE1OFF(address),
                (*pt) | PTE_VALID | PTE_RW | PTE_INMEM);
        unsetSectorUsed(diskPT);
    }
}

void loadPTE(uint32_t address, uint32_t ptFrame) {
    uint32_t diskPTE;
    uint32_t mpte = dccvmm_phy_read(ptFrame << 8 | PTE2OFF(address));
    if ((mpte & PTE_INMEM) != PTE_INMEM) {
        diskPTE = PTE_SECTOR(mpte);
        assert(diskPTE > 127);
        mpte = getFreeFrame();
        dccvmm_load_frame(diskPTE, mpte);
        dccvmm_phy_write(ptFrame << 8 | PTE2OFF(address),
                mpte | PTE_VALID | PTE_RW | PTE_INMEM);
        unsetSectorUsed(diskPTE);
    }
}