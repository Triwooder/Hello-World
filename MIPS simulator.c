#include "simulator.h"
#include "assembler.h"
#include "run.h"

computer MyCPU;
const char *regname[2] = { " avvaaaattttttttssssssssttkkgsfr", " t01012301234567012345678901pppa" };

int main() 
{    
	printf("* * * * * * * * * * * ��ӭʹ�� MIPS ģ����! * * * * * * * * * * *\n");
	printf("* ָ��˵����                                             \t*\n");
	printf("* >> a              дһ��ָ��                           \t*\n");
	printf("* >> a N            д N�������������֣���ָ��            \t*\n");
	printf("* >> a filename     ���ļ��ж�ָ��                        \t*\n");
	printf("* >> t              ִ��һ��ָ��                          \t*\n");
	printf("* >> t N            ִ�� N��������ֵ����ָ��              \t*\n");
	printf("* >> t a            ֱ����ָ�����                        \t*\n");
	printf("* >> u              ָ�ʽ���ڴ棨��PC��ʼ��            \t*\n");
	printf("* >> u addr         ָ�ʽ�� addr����ַ��ֵ����ʼ���ڴ� \t*\n");
	printf("* >> d              ���ݷ�ʽ���ڴ棨�ӵ�ַ0��ʼ��         \t*\n");
	printf("* >> d addr         ���ݷ�ʽ�� addr����ַ��ֵ����ʼ���ڴ� \t*\n");
	printf("* >> r              ���Ĵ���                             \t*\n");
	printf("* >> l filename     ���ļ��ж��ڴ�                       \t*\n");
	printf("* >> s filename     ���ڴ�д���ļ�                       \t*\n");
	printf("* >> x              �˳�                                 \t*\n");
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");

    run();
}