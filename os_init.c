#include "vmm_ext.h"
#include "vmm.h"


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