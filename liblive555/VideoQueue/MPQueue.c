/*		MPQueue.h
		视频队列操作模块
作者：wlw
时间：2011.8.15	
*/

#include <stdio.h>
#include <pthread.h>
#include "../typedef.h"
#include "MemPacket.h"
#include "MPQueue.h"





//队列未使用标记
#define _VTPOOL_UNUSE 0
//队列已用标记
#define _VTPOOL_USE 1



//功能：从内存包队列初始化
//返回值：
//	-1 – 失败
//	>= 0 –输出数据长度

int8 SMPQueue_Init(SMPQueue * queue)
{
	int32 i = 0;
	for(i = 0; i<= VTPOOL_PACKET_MAX; i++)
	{
		queue -> mp_packet[i].queNext = NULL;
		queue -> mp_packet[i].used = _VTPOOL_UNUSE;
		queue -> mp_packet[i].smpPtr = NULL;
	}
	return VTPOOL_PACKET_MAX;
}


//从内存包队列初始化
//	返回值：
//		 NULL – 失败
//   	!= NULL –成功
SMP_packet * SMPQueue_New(SMPQueue * queue)
{
	int32 i;
	for(i = 0; i <= VTPOOL_PACKET_MAX; i++)
	{
		if(queue -> mp_packet[i].used == _VTPOOL_UNUSE)
		{
			queue -> mp_packet[i].used = _VTPOOL_USE;
			return &(queue -> mp_packet[i]);
		}
	}
	return NULL;
}
//从内存包队列删除所有数据
//	输入参数：
//		qsmp – 要释放的队列指针
//	返回值：
//		-1 – 失败
//		>= 0 –输出数据长度
int8 SMPQueue_Delete(SMPQueue * queue, SMP_packet *qsmp)
{
	qsmp->used = _VTPOOL_UNUSE;
	return 0;
}
//向内存包队列中添加有效数据包
//	输入参数：
//		smp – 新分配并已经拷贝数据完成的内存数据包
//	返回值：
//		-1 – 失败
//		>= 0 –输出数据长度

int SMPQueue_Push(SMPQueue * queue, SMemPacket *smp)
{
	SMP_packet * tpmp = SMPQueue_New(queue);
	if(tpmp == NULL)
		return -1;
	tpmp-> smpPtr = smp;
	tpmp->queNext = NULL;
	if(queue -> pMPQueue == NULL)
	{
		queue -> pMPQueue = tpmp;
		
		return 0;
	}
	SMP_packet * tmp = queue -> pMPQueue;
	while(tmp -> queNext)
	{ 
		tmp = tmp -> queNext;
	}
	tmp -> queNext = tpmp;
	return 0;
}

//从内存包队列中取出数据包
//	返回值：
//		NULL – 失败
//		!= NULL –成功

SMemPacket *SMPQueue_Pop(SMPQueue * queue)
{
	SMP_packet * tpmp;
	SMemPacket * memp;
	if(queue -> pMPQueue == NULL)
	{
		return NULL;
	}
	tpmp = queue -> pMPQueue;
	memp = tpmp -> smpPtr;
	if(queue -> pMPQueue -> queNext != NULL)
	{
		queue -> pMPQueue = queue -> pMPQueue -> queNext;
	}
	else
		queue -> pMPQueue = NULL;
	SMPQueue_Delete(queue, tpmp);
	
	return memp;
	
}
//从内存包队列中取出数据，但不从队列中删除本包
//返回值：
//		>0 – 取得数据长度
//		<= 0 –取得失败

int32 SMPQueue_Clone(SMPQueue * queue)
{
	
	if(queue -> pMPQueue == NULL)
		return -1;

	return 0;
}
