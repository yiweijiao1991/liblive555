
/*SMem.h
	 
时间：2011.8.15	
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "typedef.h"
#include "Mem.h"

#define 	MAX_MEMORY_BLOCK	8

//内存管理列表
SMemory g_smemory [MAX_MEMORY_BLOCK];
//内存分配模块初始化
uint8 SMem_Init()
{
	int32 i = 0;
	for(i = 0; i< MAX_MEMORY_BLOCK; i++)
	{
		g_smemory[i].pMem = NULL;
		g_smemory[i].iSize = 0;
	}
	return 0;
}
//分配指定大小的内存
//参数
//	buffSize - 分配的内存大小
//	iType - 内存类型
//	sMsg - 内存描述
SMemory * SMem_New(uint32 buffSize, int32 iType, char * sMsg)
{
	int32 i;
	for(i = 0; i < MAX_MEMORY_BLOCK; i++)
	{
		if(g_smemory[i].iSize == 0)
			break;
	}
	if(i == MAX_MEMORY_BLOCK)
		return NULL;
	g_smemory[i].iType  = _MEM_VIRTUAL;
	g_smemory[i].iSize  = buffSize;
	strcpy(g_smemory[i].sMsg, sMsg);
	return &g_smemory[i];
}
//释放所有内存
uint8 SMem_free()
{
	int32 i;

	for(i = 0; i < MAX_MEMORY_BLOCK; i++)
	{
		if(g_smemory[i].pMem == NULL)
			continue;
			free(g_smemory[i].pMem);

	}
	 
	return 0;
}
//释放给动的内存
int32 SMem_freeby(SMemory * mem)
{

	if(mem->pMem == NULL)
		return 0;
		free(mem->pMem);
	return 0;
}
//内存整体分配
int32 GlobalSMemAlloc()
{
	int32 i;

	for(i = 0; i < MAX_MEMORY_BLOCK; i++)
	{
		if(g_smemory[i].iSize == 0)
			continue;

		g_smemory[i].pMem = malloc(g_smemory[i].iSize);
			if(g_smemory[i].pMem == NULL)
			{
				printf("malloc Failed %s\n", g_smemory[i].sMsg);
				return -1;
			}
	   printf("Memory %s [%x] size %d\n", g_smemory[i].sMsg, (unsigned int)(g_smemory[i].pMem), g_smemory[i].iSize);
	}

	printf("Memory %s [%x] size %d\n", g_smemory[i].sMsg, (unsigned int)(g_smemory[i].pMem), g_smemory[i].iSize);
	return 0;
}


