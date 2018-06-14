/*		MPQueue.h
		视频队列操作模块
作者：wlw
时间：2011.8.15	
*/

#ifndef _MPQUEUE_H
#define _MPQUEUE_H

#include "MemPacket.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define		VTPOOL_PACKET_MAX	VTMEM_PACKET_MAX

typedef struct _SMP_packet
{
	
	uint8	used;		//	使用标志	0 - 未使用（默认）
						//1 – 使用
	SMemPacket * smpPtr;//	当前数据指针	NULL（默认）
	struct _SMP_packet * queNext;	//	链表元素	NULL（默认）

}SMP_packet;

typedef struct
{
	SMP_packet   	mp_packet[VTPOOL_PACKET_MAX];//视频缓冲队列数组
	SMP_packet *	pMPQueue ; //视频缓冲队列头	NULL
	
}SMPQueue;

//从内存包队列初始化
int8 SMPQueue_Init(SMPQueue * queue);

//从内存包队列初始化
SMP_packet * SMPQueue_New(SMPQueue * queue);

//从内存包队列删除所有数据
int8 SMPQueue_Delete(SMPQueue * queue, SMP_packet *qsmp);

//向内存包队列中添加有效数据包
int SMPQueue_Push(SMPQueue * queue, SMemPacket *smp);

//从内存包队列中取出数据包
SMemPacket *SMPQueue_Pop(SMPQueue * queue);

//从内存包队列中取出数据，但不从队列中删除本包
int32 SMPQueue_Clone(SMPQueue * queue);

#ifdef __cplusplus
}
#endif

#endif
