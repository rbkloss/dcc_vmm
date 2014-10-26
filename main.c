#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vmm.h"

extern void os_init(void);
extern void os_alloc(uint32_t addr);
extern void os_free(uint32_t addr);
extern void os_swap(uint32_t pid);

#define BUFSZ 1024

int main(int argc, char **argv) {
    FILE *fd = fopen(argv[1], "r");
    char line[BUFSZ];

    os_init();
    dccvmm_init();

    while (fgets(line, BUFSZ, fd)) {
        unsigned address;
        if (line[0] == '#') continue;

        if (!strncmp(line, "alloc", 5)) {
            printf("Allocating memory ...\n");
            sscanf(line, "alloc %x\n", &address);
            os_alloc(address);
        } else if (!strncmp(line, "free", 4)) {
            printf("Freeing memory ...\n");
            sscanf(line, "free %x\n", &address);
            os_free(address);
        } else if (!strncmp(line, "read", 4)) {
            printf("Reading from memopry ...\n");
            sscanf(line, "read %x\n", &address);
            dccvmm_read(address);
        } else if (!strncmp(line, "write", 5)) {
            printf("Writing at memopry ...\n");
            unsigned data;
            sscanf(line, "write %x %x\n", &address, &data);
            dccvmm_write(address, data);
        } else if (!strncmp(line, "swap", 4)) {
            printf("Swapping processes ...\n");
            unsigned pid;
            sscanf(line, "swap %u\n", &pid);
            os_swap(pid);
        }
    }
    fclose(fd);
    exit(EXIT_SUCCESS);
}
