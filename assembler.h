#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

#include "simulator.h"
#include "convert.h"

#define NONCHR ", \n\t\r()"
#define REGCHR "$zeoavtskgpfr0123456789"
#define DECCHR "0123456789-"
#define HEXCHR "0123456789ABCDEF"

#define NIL 0
#define MAXDEC 1000000000

#define JI   0x1
#define II   0x2
#define RI   0x3
#define MI   0x4

#define INSTNUM 40

// r-inst
#define ROP     0x00
#define ADDFC	0x20
#define ADDUFC  0x21
#define SUBFC   0x22
#define SUBUFC  0x23
#define SLTFC   0x2A
#define SLTUFC  0x2B
#define ANDFC   0x24
#define ORFC    0x25
#define XORFC   0x26
#define NORFC   0x27
#define SLLFC   0x00
#define SRLFC   0x02
#define SRAFC   0x03
#define SLLVFC  0x04
#define SRLVFC  0x06
#define SRAVFC  0x07
#define JRFC    0x08
#define JALRFC  0x09
#define SYSCALLFC 0x0C

#define MULFC
#define MULTFC  0x18
#define MULTUFC 0x19
#define DIVFC   0x1A
#define DIVUFC  0x1B
#define MFHIFC  0x10
#define MFLOFC  0x12
#define MTHIFC  0x11
#define MTLOFC  0x13
// i-inst
#define LUIOP   0x0F
#define LWOP    0x23
#define SWOP    0x2B
#define LHOP    0x21
#define LHUOP   0x25
#define SHOP    0x29
#define ADDIOP  0x08
#define ADDIUOP 0x09
#define SLTIOP  0x0A
#define SLTIUOP 0x0B
#define ANDIOP  0x0C
#define ORIOP   0x0D
#define XORIOP  0x26
#define BEQOP   0x04
#define BNEOP   0x05
#define BGEZALOP 0x01

// j-inst
#define JOP     0x02
#define JALOP   0x03

// m-inst
#define LI   0x01
#define PUSH 0x03
#define POP  0x04
#define MOVE 0x05

#define PRTINT 1
#define PRTFLT 2
#define PRTDUB 3
#define PRTSTR 4

typedef struct {
    char *name;
    short type;
    uint32_t op;
} inst;

typedef struct {
	char *var[10];
	uint32_t dat[10];
	int num;
} strvar;

uint32_t lan2cod(char *asslan);
char *cod2lan(uint32_t mcode);
void upper(char *c);
uint32_t reg2num(char *reg);
uint32_t str2num(char *str);
char *num2reg(uint32_t reg);
char *num2str(uint32_t dat);
void cutInst(char *str, uint32_t *r[], int *n, int relate);
void comInst(char *str, char *r[], int mode);
inst *getInst_l(char *name);
inst *getInst_c(uint32_t mcode);
bool execute(uint32_t mcode);

#endif
