#include "disk.h"
#include "vmm_ext.h"

#define MAP_SIZE 128 //128 setores serão usados
#define NUM_WORDS 0x100 // cada setor tem 256 words
#define NUM_BITS 32 // cada word tem 32 bits
#define ZERO 0x0000000

uint32_t checkBits(uint32_t data);

void disk_init() {
    int i = 0;

    dccvmm_phy_write(SWAP_FRAME << 8, 0);

    for (i = 0; i < MAP_SIZE; i++) {
        dccvmm_dump_frame(SWAP_FRAME, i);
    }

    //uint32_t data = dccvmm_phy_read(SWAP_FRAME << 8);

    for (i = 0; i < MAP_SIZE; i++) {
        setSectorUsed(i);
    }

    // Exemplo de uso!!!
    //uint32_t sectorID = getFreeSector();
    //printf("free sector: %d\n", sectorID);
    //setSectorUsed(sectorID);
    //sectorID = getFreeSector();
    //printf("free sector: %d\n", sectorID);
    //setSectorUsed(sectorID);
}

void setSectorUsed(uint32_t sectorID) {
    // marca no mapeamento do disco que o sectorID será usado.

    uint32_t secPos = sectorID / (NUM_WORDS * NUM_BITS);
    sectorID = sectorID - secPos * (NUM_WORDS * NUM_BITS);
    uint32_t wordPos = sectorID / NUM_BITS;
    uint32_t bitPos = sectorID - wordPos*NUM_BITS;

    // printf ("secpos: %d - wordpos: %d - bitPos: %d\n", secPos, wordPos, bitPos);

    // Carrega o setor do disco na memoria;
    dccvmm_load_frame(secPos, SWAP_FRAME);

    // marca que o setor será usado;
    uint32_t data = 1 << bitPos;
    uint32_t old_data = dccvmm_phy_read(SWAP_FRAME << 8 | wordPos);
    data = data | old_data;

    // escreve de volta na memoria;
    dccvmm_phy_write(SWAP_FRAME << 8 | wordPos, data);
    // escreve novamente o setor na memoria;
    dccvmm_dump_frame(SWAP_FRAME, secPos);
}

void unsetSectorUsed(uint32_t sectorID) {
    // marca no mapeamento do disco que o sectorID será usado.
    uint32_t secPos = sectorID / (NUM_WORDS * NUM_BITS);
    sectorID = sectorID - secPos * (NUM_WORDS * NUM_BITS);
    uint32_t wordPos = sectorID / NUM_BITS;
    uint32_t bitPos = sectorID - wordPos*NUM_BITS;
    // printf ("secpos: %d - wordpos: %d - bitPos: %d\n", secPos, wordPos, bitPos);
    // Carrega o setor do disco na memoria;
    dccvmm_load_frame(secPos, SWAP_FRAME);
    // marca que o setor será usado;
    uint32_t data = 1 << bitPos;
    data = ~data;
    uint32_t old_data = dccvmm_phy_read(SWAP_FRAME << 8 | wordPos);
    data = data & old_data;
    // escreve de volta na memoria;
    dccvmm_phy_write(SWAP_FRAME << 8 | wordPos, data);
    // escreve novamente o setor na memoria;
    dccvmm_dump_frame(SWAP_FRAME, secPos);
}

uint32_t getFreeSector() {

    int i = 0;

    for (i = 0; i < MAP_SIZE; i++) {
        uint32_t sectorPos = i;
        // carrego na memoria o frame;
        dccvmm_load_frame(sectorPos, SWAP_FRAME);

        int j = 0;
        for (j = 0; j < NUM_WORDS; j++) {
            uint32_t wordPos = j;
            uint32_t data = dccvmm_phy_read(SWAP_FRAME << 8 | wordPos);

            uint32_t bit = checkBits(data);

            if (bit != 32) {
                // retorna o proximo setor livre;
                uint32_t sectorID = 8192 * sectorPos + 32 * wordPos + bit;
                setSectorUsed(sectorID);
                //                printf("Setor livre: %d\n", sectorID);
                assert(sectorID > 127);
                return sectorID;
            }
        }
    }

    // DISCO ENCHEU!!!!
    fprintf(stderr, "FULL DISK\n\n");
    assert(0);
    return -1;
}

uint32_t checkBits(uint32_t data) {
    int i = 0;
    for (i = 0; i < 32; i++) {
        if (1 & ~data)
            return i;
        else
            data = data >> 1;
    }
    return 32;
}