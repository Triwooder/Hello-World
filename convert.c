#include "convert.h"

const char *pr[OPLEVEL] = { "+-", "*/" };
extern const char *regname[2];

void upper(char *c)
{
	if (*c >= 'a' && *c <= 'z') {
		*c += 'A' - 'a';
	}
}

char *cutUnit(char *s, const char *stop, int *n)
{
	char *ts, *unit;
	ts = s;
	while (*s && strchr(stop, *s) == NULL) {
		*s++;
	}
	*n = s - ts;
	unit = (char*)malloc((*n + 1) * sizeof(char));
	strncpy(unit, ts, *n);
	unit[*n] = 0;

	return unit;
}

uint32_t reg2num(char *reg)
{
	char name[4];
	if (strcmp(reg, "$zero") == 0) {
		return 0;
	}
	for (int i = 1; i < 32; i++)
	{
		name[0] = '$';
		name[1] = regname[0][i];
		name[2] = regname[1][i];
		name[3] = 0;
		if (strcmp(reg, name) == 0) {
			return i;
		}
	}
	return 0;
}

char *num2reg(uint32_t reg)
{
	char *name;
	if (reg == 0) {
		return "$zero";
	} else {
		name = (char*)malloc(sizeof(char) * 4);
		name[0] = '$';
		name[1] = regname[0][reg];
		name[2] = regname[1][reg];
		name[3] = 0;
		return name;
	}
}

int32_t str2num(char *str)
{
	int i, scl;
	int32_t num;

	if (!str[0]) {
		return 0;
	}

	if (str[0] == '0' && str[1] == 'x') {
		i = 2;
		scl = 16;
	} else if (str[0] == '-') {
		i = 1;
		scl = 10;
	} else {
		i = 0;
		scl = 10;
	}

	for (num = 0; str[i]; i++)
	{
		num = num * scl + (uint32_t)(strchr(HEXCHR, str[i]) - strchr(HEXCHR, '0'));
	}
	num *= (str[0] == '-') ? -1 : 1;
	return num;
}

int getpr(char op)
{
	for (int i = 0; i < OPLEVEL; i++) {
		if (strchr(pr[i], op)) {
			return i;
		}
	}
}

int32_t calc(int32_t a, char op, int32_t b)
{
	switch (op) {
	case '+': return a + b;
	case '-': return a - b;
	case '*': return a * b;
	case '/': return a / b;
	default: break;
	}
}

uint32_t exp2num(char *str)
{
	char *sec;
	int32_t tnum, numstk[10];
	char opstk[10];
	int tp = 0;
	int len;

	while (*str)
	{
		sec = cutUnit(str, OPCHR, &len);
		tnum = str2num(sec);
		str += len;
		free(sec);

		numstk[tp] = tnum;

		while (tp && (!*str || getpr(*str) <= getpr(opstk[tp - 1]))) {
			numstk[tp - 1] = calc(numstk[tp - 1], opstk[tp - 1], numstk[tp]);
			tp--;
		}

		if (*str) {
			opstk[tp++] = *str++;
		}
	}
	return (uint32_t)numstk[0];
}

char *num2str(uint32_t dat)
{
	int32_t num = (int32_t)dat;
	uint32_t m = MAXDEC;
	char *str, *p;
	if (num == 0) {
		str = "0";
	} else
	{
		str = (char*)malloc(sizeof(char) * 10);
		p = str;
		if (num < 0) {
			num = -num;
			*p++ = '-';
		}
		while (m > num) {
			m /= 10;
		}
		while (m) {
			*p++ = num / m + '0';
			num = num % m;
			m /= 10;
		}
		*p = 0;
	}
	return str;
}