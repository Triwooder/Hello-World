#pragma once

#include "simulator.h"

#define DECCHR "0123456789-"
#define HEXCHR "0123456789ABCDEF"
#define OPCHR "+-*/"
#define OPLEVEL 2
#define MAXDEC 1000000000

uint32_t reg2num(char *reg);
char *num2reg(uint32_t reg);
int32_t str2num(char *str);
int getpr(char op);
int32_t calc(int32_t a, char op, int32_t b);
uint32_t exp2num(char *str);
char *num2str(uint32_t dat);
void upper(char *c); 
char *cutUnit(char *s, const char *stop, int *n);