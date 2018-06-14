/*		SMem.h
	内存分配
作者：wlw
时间：2011.8.15	
*/

#ifndef _SMEM_H
#define _SMEM_H
#include "typedef.h"
typedef struct 
{
	uint8*	pMem;//		分配的内存指针
	uint32	iSize;//	内存分配空间大小
	uint32  iType; //	内存类型
	uint32	aSize;//	内存实际使用大小
	uint16	uAlign;//	字节对齐要求
	int8	sMsg[32];//	描述内存使用类型
}SMemory, *pSMemory;

#define _MEM_VIRTUAL 0	//虚拟内存
#define _MEM_CONTINUOUS	1 //连续内存

#define  MemorySize_1M	   0x100000
#define  MemorySize_2M	   0x200000
#define  MemorySize_3M     0x300000
#define  MemorySize_6M     0x600000
#define  MemorySize_500K   0x7D000
#define  MemorySize_5M     0x500000
#define  MemorySize_12M    0xc00000
#define  MemorySize_20M    0x1400000
#ifdef __cplusplus
extern "C"
{
#endif

//内存分配模块初始化
uint8 SMem_Init();

//分配指定大小的内存
SMemory * SMem_New(uint32 buffSize, int32 iType, char * sMsg);

//释放所有内存
uint8 SMem_free();
//释放给动的内存
int32 SMem_freeby(SMemory * mem);
//内存整体分配
int32 GlobalSMemAlloc();
#ifdef __cplusplus
}
#endif

#endif
