/*		MemPacket.h
	视频内存包处理
作者：wlw
时间：2011.8.15	
*/
#include <stdio.h>
#include <string.h>
#include "../typedef.h"
#include "MemPacket.h"
#include "VideoPool.h"
 

//数组元素未使用标记
#define _VTPOOL_UNUSE 0
//数组元素已用标记
#define _VTPOOL_USE 1


//视频数组初始化

//返回值：
//	-1 – 失败
//	0L – 成功

uint8 SMemPacket_init(SMemPacket * smp)
{
	uint8 i;
	for(i = 0; i < VTMEM_PACKET_MAX; i++)
	{
		smp[i].flag = _VTPOOL_UNUSE;
	}
	return 0;
}

//分配可用的视频缓冲队列元素
//输入参数：
//	type – 帧类型，I帧或B/P帧
//返回值：
//	NULL – 失败
//	!= NULL – 成功

SMemPacket * SMemPacket_New(SMemPacket * smp, uint8 type, int32 iFrameNub, int64 time)
{
	uint8 i;
	for(i = 0; i < VTMEM_PACKET_MAX; i++)
	{
		if(smp[i].flag == _VTPOOL_UNUSE)
		{
			smp[i].flag = _VTPOOL_USE;
			smp[i].type = type;
		
			smp[i].iFrameNub = iFrameNub;
			
			return &smp[i];
		}
	}
	return NULL;
}

//释放已经分配的视频缓冲元素
//输入参数：
//		smp – 已经通过SMemPacket_New()分配的视频缓冲结构指针
//返回值：
//		-1 – 失败
//		>= 0 – 成功
int SMemPacket_Delete(SMemPacket *smp)
{
	smp-> flag = _VTPOOL_UNUSE;
	return 0;
}


uint8 SMemPacket_Print(SMemPacket * smp)
{
	uint8 i;
	for(i = 0; i < VTMEM_PACKET_MAX; i++)
	{
		printf(" %d-", smp[i].flag);
	}
	printf("\n");
	return 0;
}

/*
//对视频缓冲元素分配内存
//输入参数：
//		smp – 已经通过SMemPacket_New()分配的视频缓冲结构指针
//		size – 分配的大小
//返回值：
//		-1 – 失败
//		>= 0 – 成功
int8 SMemPacket_Alloc(SVideoPool * pVieoPool, SMemPacket *smp, int32 size)
{
	smp->memptr = SVideoPool_GetBuf(pVieoPool, size);
	if(smp->memptr == NULL)
	{
		return -1;
	}
	smp->memsize = size;
	return 0;
}

//对视频缓冲元素释放内存
int8 SMemPacket_Free(SMemPacket *smp)
{
	if(SVideoPool_FreeBuf(smp -> memptr, smp -> memsize) < 0)
		return -1;
	return 0;
}

//将数据写入包结构
//输入参数：
//		smp – 已经通过SMemPacket_New()分配的视频缓冲结构指针
//		dPtr – 输入数据缓冲区
//		dSize – 输入数据大小
//返回值：
//		-1 – 失败
//		>= 0 – 成功

int8 SMemPacket_SetData(SMemPacket * smp, uint8 *dPtr, int32 dSize)
{
	SVideoPool_Write(pVieoPool, smp -> memptr, dPtr, dSize);
	smp -> memsize = dSize;
	return 0;
}

//从包结构中取出数据
//输入参数：
//		smp – 已经通过SMemPacket_New()分配的视频缓冲结构指针
//		dPtr – 输出数据缓冲区
//返回值：
//		-1 – 失败
//		> 0 –输出数据长度
int32 SMemPacket_GetData(SVideoPool * pVieoPool, SMemPacket * smp, uint8 *dPtr)
{
	SVideoPool_Read(pVieoPool, dPtr, smp -> memptr, smp -> memsize);
	return 0;
}
*/
