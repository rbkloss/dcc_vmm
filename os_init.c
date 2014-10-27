#include "vmm_ext.h"
#include "vmm.h"
#include "disk.h"

void os_init(void) {
    //set the free frames

    disk_init();

    outOfMemory_ = FALSE;
    freesStart_ = 16;
    procTable_ = 1;
    diskProcTable_ = 3;
    int i = 0;
    for (i = 0; i < NUMFRAMES; i++) {
        dccvmm_zero(i);
    }
    uint32_t addr;
    for (i = 16; i < NUMFRAMES - 1; i++) {
        addr = i << 8;
        dccvmm_phy_write(addr, i + 1);
    }
    addr = i << 8;
    dccvmm_phy_write(addr, 0); //write that the last freeFrame points to nil    
    //set page table
    dccvmm_set_page_table(0);
}
