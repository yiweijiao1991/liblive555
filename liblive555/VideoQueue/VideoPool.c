/*		VideoPool.h
	视频传输缓冲池处理子模块
作者：wlw
时间：2011.8.15	
*/
#include <stdio.h>
#include <string.h>
#include "../typedef.h"
#include "VideoPool.h"


//视频传输缓冲池初始化
//输入参数：
//		pVieoPool – 需要初始化的缓冲池
//		memPtr – 输入缓冲池指针
//		memSize – 输入缓冲池长度
//返回值：
//		-1 – 失败
//		>= 0 – 成功

int32 SVideoPool_Init(SVideoPool * pVieoPool,uint8 *memPtr, int32 memSize)
{
	if(memPtr == NULL)
		return -1;
	pVieoPool -> memptr = memPtr;
	pVieoPool -> maxsize = memSize;
	pVieoPool -> readptr = 0;
	pVieoPool -> writeptr = 0;
	pVieoPool -> allocated = 0;
	pVieoPool -> unused = pVieoPool -> maxsize;
	return 0;
}
//从视频传输缓冲池中分配一块内存数据
//输入参数：
//		pVieoPool – 分配的内存所在的缓冲池
//		size – 需要分配的缓冲区大小
//返回值：
//	NULL – 失败
//	!=NULL – 成功

uint8 * SVideoPool_GetBuf(SVideoPool * pVieoPool, int32 size)
{
	int32 tmpptr;
	if(pVieoPool -> unused < size)
	{
		return NULL;
	}
	pVieoPool -> unused -= size;
	pVieoPool -> allocated += size;
	tmpptr = pVieoPool -> writeptr;
	pVieoPool -> writeptr = (pVieoPool -> writeptr + size) % pVieoPool -> maxsize;
	return (pVieoPool -> memptr + tmpptr);
}

//从视频传输缓冲池中释放指定的内存
//输入参数：
//		pVieoPool – 释放的内存所在的缓冲池
//		buf – 已经分配的缓冲区指针
//		size – 需要分配的缓冲区大小
//返回值：
//		NULL – 失败
//		!=NULL – 成功
int  SVideoPool_FreeBuf(SVideoPool * pVieoPool, uint8 *buf, int32 size)
{
	if(buf == NULL)
		return -1;
	pVieoPool -> readptr = (pVieoPool -> readptr + size) % pVieoPool -> maxsize;
	pVieoPool -> unused += size;
	pVieoPool -> allocated -= size;
	return 0;
}

//向视频缓冲区中拷贝数据
//输入参数：
//		pVideoPool －操作的缓冲池
//		dst – 目的指针
//		src – 源指针
//		size－ 大小
//返回值：
//		-1 – 失败
//		0  － 成功
int  SVideoPool_Write(SVideoPool * pVieoPool, uint8 *dst, uint8 * src, int32 size)
{
	if((pVieoPool -> writeptr - size) >= 0)
		memcpy(dst, src, size);
	else
	{
		int32 iOff = pVieoPool -> maxsize -(dst - pVieoPool -> memptr);
		memcpy(dst, src, iOff);
		memcpy(pVieoPool -> memptr, src + iOff, pVieoPool -> writeptr);
	}
	return 0;
}
//从视频缓冲区中向外拷贝数据
//输入参数：
//		pVideoPool －操作的缓冲池
//		dst – 目的指针
//		src – 源指针
//		size－ 大小
//返回值：
//		-1 – 失败
//		0  － 成功
int SVideoPool_Read(SVideoPool * pVieoPool, uint8 *dst, uint8 *src, int32 size)
{	
	
	if(pVieoPool -> maxsize  >= (src - pVieoPool -> memptr + size))
		memcpy(dst, src, size);
	else
	{
		int32 iOff = pVieoPool -> maxsize - (src  - pVieoPool -> memptr) ;
		memcpy(dst, src, iOff);
		memcpy(dst + iOff, pVieoPool -> memptr, size - iOff);
	}
	return 0;
}
