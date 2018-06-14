/*		VTInterf.h
视频传输模块连接视频处理模块和通讯模块中的视频数据传输模块，
采用共享公用缓冲池的方式实现两个视频模块与传输模块之间的数据传输

作者：wlw
时间：2011.8.15	
*/

#ifndef _VITNTERF_H
#define _VTINTERF_H
#include "../Mem.h"
#include "VideoPool.h"
#include "MemPacket.h"
#include "MPQueue.h"
#include <pthread.h>

#define _FRAME_TYPE_I 	0
#define _FRAME_TYPE_P	1
typedef struct 
{
	SMemory *	mem_vtinterf;	//缓冲池内存描述 
	SVideoPool	video_pool; //	视频缓冲池
	SMemPacket	mp_array[VTMEM_PACKET_MAX]; //	视频内存包数组
	SMPQueue	mp_queue; //	视频队列操作结构体
	pthread_mutex_t VT_mut; //线程互斥体
}SVTInterf, *pSVTInterf;

typedef struct
{
	int32 length;		//帧长度
	int8 type;			//帧类型
	int32 iFrameNub; 	//帧序号
	int64 iTime;		//时间
	byte * pData;		//帧数据
}frame_info;

#ifdef __cplusplus
extern "C"
{
#endif


//视频传输接口模块初始化
int32 VTInterf_Init(SVTInterf * pVtInterf, SMemory *MemPool);

//向视频缓冲池中加入视频数据
int32 VTInterf_PutData(SVTInterf * pVtInterf, frame_info * frame);


//从视频缓冲池中取出视频数据
int32 VTInterf_GetData(SVTInterf * pVtInterf, frame_info * frame);

//丢弃所有帧数据
int32 VTInterf_DelPFream(SVTInterf * pVtInterf);

//丢弃一帧数据
int32 VTInterf_DelOneFream(SVTInterf * pVtInterf);

#ifdef __cplusplus
}
#endif

#endif
