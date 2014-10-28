#include "vmm_ext.h"
#include "vmm.h"
#include "disk.h"

void os_init(void) {
    //set the free frames    
    outOfMemory_ = FALSE;
    freesStart_ = 2;
    procTable_ = 0;
//    diskProcTable_ = 3;
    int i = 0;
    for (i = 0; i < NUMFRAMES; i++) {
        dccvmm_zero(i);
    }
    uint32_t addr;
    for (i = freesStart_; i < NUMFRAMES - 1; i++) {
        addr = i << 8;
        dccvmm_phy_write(addr, i + 1);
    }
    addr = i << 8;
    dccvmm_phy_write(addr, 0); //write that the last freeFrame points to nil    
    //set page table
    disk_init();
    os_swap(0);
}
