#include "vmm_ext.h"
#include "vmm.h"

void os_init(void) {
    //set the free frames
    outOfMemory_ = FALSE;
    freesStart_ = 2;
    procTable_ = 1;
    int i = 0;
    uint32_t addr;
    for (i = 2; i < NUMFRAMES - 1; i++) {
        addr = i << 8;
        dccvmm_phy_write(addr, i + 1);
    }
    addr = i << 8;
    dccvmm_phy_write(addr, 0); //write that the last freeFrame points to nil    
    //set page table
    dccvmm_set_page_table(0);
}
