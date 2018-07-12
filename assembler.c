#include "assembler.h"

extern computer MyCPU;
extern const char *regname[2];

union {
	float f;
	uint32_t d;
} intflt;
union {
	float lf;
	uint32_t d;
} intdbl;

static const inst Insts[INSTNUM] = {
    { "ADD", RI, ADDFC },
    { "ADDU", RI, ADDUFC },
    { "SUB", RI, SUBFC },
    { "SUBU", RI, SUBUFC },
    { "SLT", RI, SLTFC },
    { "SLTU", RI, SLTUFC },
    { "AND", RI, ANDFC },
    { "OR", RI, ORFC },
    { "XOR", RI, XORFC },
    { "NOR", RI, NORFC },
    { "SLL", RI, SLLFC },
    { "SRL", RI, SRLFC },
    { "SRA", RI, SRAFC },
    { "SLLV", RI, SLLVFC },
    { "SRLV", RI, SRLVFC },
    { "SRAV", RI, SRAVFC },
    { "JR", RI, JRFC },
    { "JALR", RI, JALRFC },
    { "SYSCALL", RI, SYSCALLFC },
    // I-instructions
    { "LUI" , II, LUIOP },
    { "LW" , II, LWOP },
    { "SW", II, SWOP },
    { "LH" , II, LHOP },
    { "LHU" , II, LHUOP },
    { "SH", II, SHOP },
    { "ADDI" , II, ADDIOP },
    { "ADDIU" , II, ADDIUOP },
    { "SLTI" , II, SLTIOP },
    { "SLTIU" , II, SLTIUOP },
    { "ANDI" , II, ANDIOP },
    { "ORI"  , II, ORIOP },
    { "XOIR" , II, XORIOP },
    { "BEQ"  , II, BEQOP },
    { "BNE"  , II, BNEOP },
    { "BGEZALOP" , II, BGEZALOP },
    // j-instructions
    { "J", JI, JOP },
    { "JAL", JI, JALOP}
	// m-instructions
    //{ "LI", MI, LI },
    //{ "PUSH", MI, PUSH },
    //{ "POP", MI, POP },
    //{ "MOVE", MI, MOVE }
};
strvar sv;


inst *getInst_l(char *name) 
{
    char *ptr = name;
    
    while (*ptr) {
        upper(ptr++);
    }

    for (int i = 0; i < INSTNUM; i++) {
        if (strcmp(Insts[i].name, name) == 0) {
            return &Insts[i];
        }
    }
    return NULL;
}

inst *getInst_c(uint32_t mcode)
{
    uint32_t opcode, func;
    opcode = (mcode >> 26) & 0x3F;
    func = mcode & 0x3F;
    if (opcode == 0) {
        for (int i = 0; i < INSTNUM; i++) {
            if (Insts[i].type == RI && Insts[i].op == func) {
                return &Insts[i];
            }
        }
    } else {
        for (int i = 0; i < INSTNUM; i++) {
            if (Insts[i].type != RI && Insts[i].op == opcode) {
                return &Insts[i];
            }
        }
    }
    return NULL;
    
}

void cutInst(char *str, uint32_t *r[], int *n, int relate) 
{    
    char *sec;

    *n = 0;
    while (*str) 
    {
        if (strchr(NONCHR, *str)) {
            str++;
            continue;
        }

		int len;
        sec = cutUnit(str, NONCHR, &len);
        if (*sec == '$') {
            *r[(*n)++] = reg2num(sec);
        } else if (strchr(DECCHR, *sec)) {
            *r[(*n)++] = exp2num(sec);
		} else {
			for (int i = 0; i < sv.num; i++) {
				if (strcmp(sec, sv.var[i]) == 0) {
					*r[(*n)++] = (uint32_t)(sv.dat[i] - relate * (MyCPU.MemAdr + 4));
					break;
				}
			}
		}
        str += len;
        free(sec);
    }
}

void comInst(char *str, char *r[], int mode)
{
    int i;
    for (i = 0; r[i] && i < 3; i++) {
		if (i == 0) {
			strcat(str, " ");
		} else if (mode && i == 2) {
            strcat(str, "(");
        } else {
            strcat(str, ", ");
        }
        strcat(str, r[i]);
        if (mode && i == 2) {
            strcat(str, ") ");
        }
    }
}

uint32_t lan2cod(char *asslan) 
{
    uint32_t mcode, rs, rt, rd, sa, dat, adr;
    inst *tinst;
    char *str;
    uint32_t *r[3];
    int len, relate;

    for (; strchr(NONCHR, *asslan); asslan++);

	str = cutUnit(asslan, NONCHR, &len);
	asslan += len;
	if (str[len - 1] == ':') 
	{
		str[len - 1] = 0;
		sv.var[sv.num] = str;
		sv.dat[sv.num] = MyCPU.MemAdr;
		sv.num++;

		for (; strchr(NONCHR, *asslan); asslan++);
		str = cutUnit(asslan, NONCHR, &len);
		asslan += len;
	}
    tinst = getInst_l(str);
    if (tinst == NULL) {
        return 0;
    }
    free(str);

    int n;
    switch (tinst->type) {
    case RI:
        switch (tinst->op) {
		case SLLFC: case SRLFC: case SRAFC:
            r[0] = &rd, r[1] = &rs, r[2] = &sa;
			rt = 0;
            break;
		case JRFC:
			r[0] = &rs;
			rt = 0, rd = 0, sa = 0;
			break;
		case SYSCALLFC:
			rs = 0, rt = 0, rd = 0, sa = 0;
			break;
        default:
            r[0] = &rd, r[1] = &rs, r[2] = &rt;
			sa = 0;
            break;
        }
        cutInst(asslan, r, &n, 0);
        mcode = (0 << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (sa << 6) | (tinst->op);
        break;
    case II:
        switch (tinst->op) {
        case LUIOP:
            r[0] = &rt, r[1] = &dat;
            rs = 0;
			relate = 1;
            break;
        case LWOP: case LHOP: case LHUOP: case SWOP: case SHOP:
            r[0] = &rt, r[1] = &dat, r[2] = &rs;
			relate = 1;
            break;
        case ADDIOP: case ADDIUOP: case SLTIOP: case SLTIUOP: case ANDIOP: case ORIOP: case XORIOP:
			r[0] = &rt, r[1] = &rs, r[2] = &dat;
			relate = 0;
			break;
		case BEQOP: case BNEOP:
            r[0] = &rt, r[1] = &rs, r[2] = &dat;
			relate = 1;
            break;
        case BGEZALOP:
            r[0] = &rs, r[1] = &dat;
            rt = 17;
			relate = 1;
            break;
        }
        cutInst(asslan, r, &n, relate);
        mcode = ((tinst->op & 0x3F) << 26) | (rs << 21) | (rt << 16) | (uint16_t)dat;
        break;
    case JI:
        r[0] = &adr;
        cutInst(asslan, r, &n, 0);
        mcode = ((tinst->op & 0x3F) << 26) | (adr & 0x3FFFFFF);
        break;
	/*case MI:
		switch (tinst->op) {
		case LI:
			r[0] = &rs, r[1] = &dat;
			cutInst(asslan, r, &n, 0);
			if ((dat >> 16) & 0xFFFF == 0 || (dat >> 16) & 0xFFFF == 0xFFFF) {
				str = (char)malloc(sizeof(char) * 50);
				strcpy(str, "ADDI");
				t[0] = num2reg(rs), t[1] = num2reg(0), t[2] = num2str(dat);
				comInst(str, t, 0);
				mcode = lan2cod(str);
				free(str);
			} else {

			}
		}*/
    }

    return mcode;
}

char *cod2lan(uint32_t mcode)
{
    if (!mcode) {
        return NULL;
    }
    uint32_t rs, rt, dat, rd, sa, adr;
    inst *tinst;
    char *r[3];
    char *asslan = (char*)malloc(sizeof(char) * 50);

    rs = (mcode >> 21) & 0x1F;
    rt = (mcode >> 16) & 0x1F;
    rd = (mcode >> 11) & 0x1F;
	sa = (mcode >> 6) & 0x1F;

    tinst = getInst_c(mcode);
    if (tinst == NULL) {
        return NULL;
    }
	strcpy(asslan, tinst->name);

    switch (tinst->type) {
    case RI:
		switch (tinst->op) {
		case SLLFC: case SRLFC: case SRAFC:
			r[0] = num2reg(rd), r[1] = num2reg(rs), r[2] = num2str(sa);
			break;
		case JRFC:
			r[0] = num2reg(rs), r[1] = NULL, r[2] = NULL;
			break;
		case SYSCALLFC:
			r[0] = NULL, r[1] = NULL, r[2] = NULL;
			break;
		default:
			r[0] = num2reg(rd), r[1] = num2reg(rs), r[2] = num2reg(rt);
			break;
		}
        comInst(asslan, r, 0);
        break;
    case II:
        dat = (uint32_t)(int32_t)(int16_t)(mcode & 0xFFFF);
        switch (tinst->op) {
        case LUIOP:
            r[0] = num2reg(rt), r[1] = num2str(dat), r[2] = NULL;
            comInst(asslan, r, 0);
            break;
        case LWOP:case LHOP:case LHUOP:case SWOP:case SHOP:
            r[0] = num2reg(rt), r[1] = num2str(dat), r[2] = num2reg(rs);
            comInst(asslan, r, 1);
            break;
        case ADDIOP:case ADDIUOP:case SLTIOP:case SLTIUOP:case ANDIOP:case ORIOP:case XORIOP:case BEQOP:case BNEOP:
            r[0] = num2reg(rt), r[1] = num2reg(rs), r[2] = num2str(dat);
            comInst(asslan, r, 0);
            break;
        case BGEZALOP:
            r[0] = num2reg(rs), r[1] = num2str(dat), r[2] = NULL;
            comInst(asslan, r, 0);
            break;
        }
        break;
    case JI:
        adr = mcode & 0x3FFFFFF;
        r[0] = num2str(adr), r[1] = NULL, r[2] = NULL;
        comInst(asslan, r, 0);
        break;
    }

    return asslan;
}

bool execute(uint32_t mcode)
{
    uint32_t rs, rt, dat, rd, sa, adr, ind, ofs;
	uint8_t chr;
    inst *tinst;
	int i;
    tinst = getInst_c(mcode);
    if (tinst == NULL) {
        return false;
    }
    rs = (mcode >> 21) & 0x1F;
    rt = (mcode >> 16) & 0x1F;

    switch (tinst->type) {
    case RI:
        rd = (mcode >> 11) & 0x1F;
        sa = (mcode >> 6) & 0x1F;
        switch (tinst->op) {
        case ADDFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] + MyCPU.Rgf[rt];
            break;
        case ADDUFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] + MyCPU.Rgf[rt];
            break;
        case SUBFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] - MyCPU.Rgf[rt];
            break;
        case SUBUFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] - MyCPU.Rgf[rt];
            break;
        case SLTFC:
            MyCPU.Rgf[rd] = ((int32_t)MyCPU.Rgf[rs] < (int32_t)MyCPU.Rgf[rt]) ? 1 : 0;
            break;
        case SLTUFC:
            MyCPU.Rgf[rd] = (MyCPU.Rgf[rs] < MyCPU.Rgf[rt]) ? 1 : 0;
            break;
        case ANDFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] & MyCPU.Rgf[rt];
            break;
        case ORFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] | MyCPU.Rgf[rt];
            break;
        case XORFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] ^ MyCPU.Rgf[rt];
            break;
        case NORFC:
            MyCPU.Rgf[rd] = ~(MyCPU.Rgf[rs] | MyCPU.Rgf[rt]);
            break;
        case SLLFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] << sa;
            break;
        case SLLVFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] <<MyCPU.Rgf[rt];
            break;
        case SRLFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] >> sa;
            break;
        case SRLVFC:
            MyCPU.Rgf[rd] = MyCPU.Rgf[rs] >> MyCPU.Rgf[rt];
            break;
        case SRAFC:
            MyCPU.Rgf[rd] = (int32_t)MyCPU.Rgf[rs] >> sa;
            break;
        case SRAVFC:
            MyCPU.Rgf[rd] = (int32_t)MyCPU.Rgf[rs] >> MyCPU.Rgf[rt];
            break;
        case JRFC:
            MyCPU.PC = MyCPU.Rgf[rs];
            break;
        case JALRFC:
            MyCPU.Rgf[rd] = MyCPU.PC; 
            MyCPU.PC = MyCPU.Rgf[rs];
            break;
		case SYSCALLFC:
			switch (MyCPU.Rgf[2]) {
			case PRTINT:
				printf("%d", MyCPU.Rgf[4]);
				break;
			case PRTFLT:
				intflt.d = MyCPU.Rgf[4];
				printf("%f", intflt.f);
				break;
			case PRTDUB:
				intdbl.d = MyCPU.Rgf[4];
				printf("%lf", intdbl.lf);
				break;
			case PRTSTR:				
				for (i = MyCPU.Rgf[4]; chr = (char)((MyCPU.Memory[i / 2] >> (i % 2 ? 0 : 8)) & 0xFF); i++) {
					printf("%c", chr);
				}
			}
			break;
        }
        break;
    case II:
        dat = (uint32_t)(int32_t)(int16_t)(mcode & 0xFFFF);
        switch (tinst->op) {
        case LUIOP:
            MyCPU.Rgf[rt] = dat << 16;
            break;
        case LWOP:
            MyCPU.Rgf[rt] = (MyCPU.Memory[MyCPU.Rgf[rs] + dat / 2] << 16) | MyCPU.Memory[MyCPU.Rgf[rs] + dat / 2 + 1];
            break;
        case LHOP:
            MyCPU.Rgf[rt] = MyCPU.Memory[MyCPU.Rgf[rs] + dat];
            break;
        case LHUOP:
            MyCPU.Rgf[rt] = MyCPU.Memory[MyCPU.Rgf[rs] + dat];
            break;
        case SWOP:
            MyCPU.Memory[MyCPU.Rgf[rs] + dat / 2] = (MyCPU.Rgf[rt] >> 16) & 0xFFFF;
            MyCPU.Memory[MyCPU.Rgf[rs] + dat / 2 + 1] = MyCPU.Rgf[rt] & 0xFFFF;
            break;
        case SHOP:
            MyCPU.Memory[MyCPU.Rgf[rs] + dat] = MyCPU.Rgf[rt];
            break;
        case ADDIOP:
            MyCPU.Rgf[rt] = MyCPU.Rgf[rs] + dat;
            break;
        case ADDIUOP:
            MyCPU.Rgf[rt] = MyCPU.Rgf[rs] + dat;
            break;
        case SLTIOP:
            MyCPU.Rgf[rt] = ((int32_t)MyCPU.Rgf[rs] < (int32_t)dat) ? 1 : 0;
            break;
        case SLTIUOP:
            MyCPU.Rgf[rt] = (MyCPU.Rgf[rs] < dat) ? 1 : 0;
            break;
        case ANDIOP:
            MyCPU.Rgf[rt] = MyCPU.Rgf[rs] & dat;
            break;
        case ORIOP:
            MyCPU.Rgf[rt] = MyCPU.Rgf[rs] | dat;
            break;
        case XORIOP:
            MyCPU.Rgf[rt] = MyCPU.Rgf[rs] ^ dat;
            break;
        case BEQOP:
            if (MyCPU.Rgf[rs] == MyCPU.Rgf[rt]) {
                MyCPU.PC += (int32_t)dat;
            }
            break;
        case BNEOP:
            if (MyCPU.Rgf[rs] != MyCPU.Rgf[rt]) {
                MyCPU.PC += (int32_t)dat;
            }
            break;
        case BGEZALOP:
            if (MyCPU.Rgf[rs] >= 0) {
                MyCPU.Rgf[31] = MyCPU.PC; 
                MyCPU.PC += dat;
            }
            break;
        }
        break;
    case JI:
        adr = mcode & 0x3FFFFFF;
        switch (tinst->op) {
        case JOP:
            MyCPU.PC = (MyCPU.PC & 0xF8000000) | adr;
            break;
        case JALOP:
            MyCPU.Rgf[31] = MyCPU.PC;
            MyCPU.PC = (MyCPU.PC & 0xF8000000) | adr;
            break;
        }
        break;
    }
    return true;
}