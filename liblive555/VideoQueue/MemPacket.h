/*		MemPacket.h
	视频内存包处理
作者：wlw
时间：2011.8.15	
*/

#ifndef _MEMPACKET_H
#define _MEMPACKET_H
#define		VTMEM_PACKET_MAX	20

typedef struct
{
	uint8	flag;	//占用标志	0 – 未占用(默认)
					//1 – 已占用
	uint8	type;	//类型	0 – I-Frame
					//1 –P/B-Frame
	uint8*	memptr; //缓冲包指针	NULL(默认)
	int32	memsize;//缓冲包大小	0(默认)
	int64 	iTime; //时间
	int32 	iFrameNub; //帧序号
}SMemPacket;

#ifdef __cplusplus
extern "C"
{
#endif




//视频数组初始化
uint8 SMemPacket_init(SMemPacket * smp);
//分配可用的视频缓冲队列元素
SMemPacket * SMemPacket_New(SMemPacket * smp, uint8 type, int32 iFrameNub, int64 time);

//释放已经分配的视频缓冲元素
int SMemPacket_Delete(SMemPacket * smp);
/*
//对视频缓冲元素分配内存
int8 SMemPacket_Alloc(SVideoPool * pVieoPool, SMemPacket *smp, int32 size);

//对视频缓冲元素释放内存
int8 SMemPacket_Free(SVideoPool * pVieoPool, SMemPacket *smp);

//将数据写入包结构
int8 SMemPacket_SetData(SVideoPool * pVieoPool, SMemPacket * smp, uint8 *dPtr, int32 dSize);

//从包结构中取出数据
int32 SMemPacket_GetData(SVideoPool * pVieoPool, SMemPacket * smp, uint8 *dPtr);*/
#ifdef __cplusplus
}
#endif

#endif
