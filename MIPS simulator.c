#include "simulator.h"
#include "assembler.h"
#include "run.h"

computer MyCPU;
const char *regname[2] = { " avvaaaattttttttssssssssttkkgsfr", " t01012301234567012345678901pppa" };

int main() 
{    
	printf("* * * * * * * * * * * 欢迎使用 MIPS 模拟器! * * * * * * * * * * *\n");
	printf("* 指令说明：                                             \t*\n");
	printf("* >> a              写一条指令                           \t*\n");
	printf("* >> a N            写 N（任意整型数字）条指令            \t*\n");
	printf("* >> a filename     从文件中读指令                        \t*\n");
	printf("* >> t              执行一条指令                          \t*\n");
	printf("* >> t N            执行 N（整型数值）条指令              \t*\n");
	printf("* >> t a            直行至指令结束                        \t*\n");
	printf("* >> u              指令方式读内存（从PC开始）            \t*\n");
	printf("* >> u addr         指令方式从 addr（地址数值）开始读内存 \t*\n");
	printf("* >> d              数据方式读内存（从地址0开始）         \t*\n");
	printf("* >> d addr         数据方式从 addr（地址数值）开始读内存 \t*\n");
	printf("* >> r              读寄存器                             \t*\n");
	printf("* >> l filename     从文件中读内存                       \t*\n");
	printf("* >> s filename     将内存写入文件                       \t*\n");
	printf("* >> x              退出                                 \t*\n");
	printf("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *\n");

    run();
}