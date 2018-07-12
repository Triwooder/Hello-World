#include "run.h"

extern computer MyCPU;
extern const char *regname[2];

void dispRgf() 
{
    for (int i = 0; i < 14; i++) {
        putchar(' ');
    }
    for (int i = 1; i < 32; i++) {
        if (i % 8 == 0) {
            printf("\n");
        }
        printf("  %c%c: %08X", regname[0][i], regname[1][i], MyCPU.Rgf[i]);
    }
    putchar('\n');
}

void dispMem_vdat(uint32_t addr) 
{    
    uint32_t index, ofs;
	uint8_t cdigt;
    for (int i = 0; i < 16; addr += 16, i++) 
	{
        printf("  0x%04X:  ", addr);

        for (int j = 0; j < 16; j++) {
            index = (addr + j) / 2;
            ofs = (addr + j) % 2;
            printf("%02X ", (uint8_t)(MyCPU.Memory[index] >> (ofs ? 0 : 8)));
			if (j % 8 == 7) {
				putchar(' ');
			}
        }
		putchar('\t');

		for (int j = 0; j < 16; j++) {
			index = (addr + j) / 2;
			ofs = (addr + j) % 2;
			cdigt = (uint8_t)(MyCPU.Memory[index] >> (ofs ? 0 : 8));
			if (cdigt > 32 && cdigt < 126) {
				putchar(cdigt);
			} else {
				putchar('.');
			}
		}
        putchar('\n');
    }
}

void dispMem_vins(uint32_t addr) 
{
    uint32_t Inst, index;
    char *s;
    for (int i = 0; i < 16 || s; i++) {
        index = (addr + i * 4) / 2;
        Inst = (MyCPU.Memory[index] << 16) & 0xFFFF0000 | MyCPU.Memory[index + 1] & 0xFFFF;
        s = cod2lan(Inst);
        if (i < 16 && s == NULL) {
            printf("  0x%04X: \n", addr + i * 4);
		} else if (s == NULL) {
			break;
		} else {
            printf("  0x%04X: %-25s\t", addr + i * 4, s);
			if (MyCPU.PC == addr + i * 4) {
				printf("PC");
			}
			putchar('\n');
        }
    }
}

bool writInst() 
{
    uint32_t mcode;
    char asslan[50];
	if (gets(asslan) == 0) {
		return false;
	}
    mcode = lan2cod(asslan);
    if (!mcode) {
        return false;
    }
    MyCPU.Memory[MyCPU.MemAdr / 2] = (uint16_t)(mcode >> 16);
    MyCPU.Memory[MyCPU.MemAdr / 2 + 1] = (uint16_t)(mcode);
    MyCPU.MemAdr += 4;
    return true;
}

bool runInst()
{
    uint32_t mcode, index;
    index = MyCPU.PC / 2;
    mcode = (MyCPU.Memory[index] << 16) & 0xFFFF0000 | MyCPU.Memory[index + 1] & 0xFFFF;
    MyCPU.PC += 4;
    if (execute(mcode) == true && mcode) {
        //printf("  %-25s\tPC 0x%04X\n", cod2lan(mcode), MyCPU.PC);
        return true;
    } else {
        MyCPU.PC -= 4; 
        //printf("Executed Completely\n");
        return false;
    }
}

void loadMem(const char* file) 
{
	int i;
	char c;
	uint8_t t = 0U;
	uint32_t index, ofs;

	freopen(file, "r", stdin);
	for (i = 0, c = getchar(); c && i < 4 * K; i++, c = getchar()) {
		t |= (uint8_t)(strchr(HEXCHR, c) - strchr(HEXCHR, '0')) << (i % 2 ? 0 : 4);
		if (i % 2) 
		{
			index = i / 4;
			ofs = i / 2 % 2;
			MyCPU.Memory[index] |= t << (ofs ? 0 : 8);
			t = 0U;
		}
	}
	fclose(stdin);
	freopen("CON", "r", stdin);
}

void storMem(const char* file)
{
	int i;
	char c;
	uint8_t t = 0U;
	uint32_t index, ofs;

	freopen(file, "w", stdout);
	for (i = 0; i < 2 * K; i++) {
		printf("%04X", MyCPU.Memory[i]);
	}
	fclose(stdout);
	freopen("CON", "w", stdout);
}

void reset()
{
	MyCPU.PC = 0;
	MyCPU.MemAdr = 0;
	for (int i = 0; i < 2 * K; i++) {
		MyCPU.Memory[i] = 0;
	}
	for (int i = 0; i < 32; i++) {
		MyCPU.Rgf[i] = 0;
	}
}

void run()
{
    int n;
    char s[10];
    char *c, *file = NULL;

	reset();

    for (;;) {
        printf(">> ");
        gets(s);   

        c = &s[1];
        while (*c && strchr(NONCHR, *c)) {
            c++;
        }
        
        switch (s[0]) {
        case 'r': case 'R':
            dispRgf();
            break;
        case 'd': case 'D':
            if (!*c) {
                dispMem_vdat(INSTMEM);
            } else if (*c && strchr(DECCHR, *c)) {
                n = str2num(c);
                dispMem_vdat(n);
            }
            break;
        case 'u': case 'U':
			if (!*c) {
				dispMem_vins(MyCPU.PC);
			} else if (*c && strchr(DECCHR, *c)) {
				n = str2num(c);
				dispMem_vins(n);
			}
            break;
        case 'a': case 'A':
            if (!*c) {
                writInst();
            } else if (*c && strchr(DECCHR, *c)) {
                n = str2num(c);
                for (; n > 0 && writInst() == true; n--);
            } else {
                freopen((const char*)c, "r", stdin);
				while (writInst() == true);
                fclose(stdin);
                freopen("CON", "r", stdin);
            }
            break;
        case 't': case 'T':
            if (!*c) {
                runInst();
            } else if (*c && strchr(DECCHR, *c)) {
                n = str2num(c);
                for (; n > 0 && runInst() == true; n--);
            } else if (*c == 'a') {
                while (runInst() == true);
            }
            break;
		case 'l': case 'L':
			loadMem((const char*)c);
			break;
		case 's': case 'S':
			storMem((const char*)c);
			break;
		case 'e': case 'E':
			reset();
			break;
        case 'x': case 'X':
            exit(0);
            break;
        default:
            break;
        }
    }   
}