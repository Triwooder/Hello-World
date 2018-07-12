#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define B 8
#define K 1024
#define INSTMEM 0
#define DATAMEM 0x400

typedef struct {
    uint32_t PC;
	uint32_t MemAdr;
    uint16_t Memory[2 * K];
    uint32_t Rgf[32];
} computer;
typedef enum {
    true, false
} bool;
#endif
