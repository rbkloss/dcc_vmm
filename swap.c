#include "vmm_ext.h"
#include "vmm.h"

void os_swap(uint32_t pid) {
    //look for the frame of the process page table
    // if not on the memory, load from disk
    //call function to set the pagetable
    printf("Swapping processes ...\n");
    uint32_t processDir = dccvmm_phy_read(procTable_ << 8 | pid);
    if (processDir & PTE_INMEM) {
        printf("process is in memory\n");
    } else {
        if (processDir & PTE_VALID) {
            //process exists but is in the disk
            //TODO
        } else {
            printf("New Process\n");
            processDir = getFreeFrame();            
            dccvmm_phy_write(PTEFRAME(procTable_) << 8 | pid,
                    processDir | PTE_VALID | PTE_INMEM);
        }
    }
    dccvmm_set_page_table(PTEFRAME(processDir));
    currentPID_ = pid;
}

