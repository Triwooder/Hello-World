#ifndef _RUN_H_
#define _RUN_H_

#include "simulator.h"
#include "assembler.h"
#include "convert.h"


void dispRgf();
void dispMem_vdat(uint32_t addr);
void dispMem_vins(uint32_t addr);
bool writInst();
bool runInst();
void run();


#endif