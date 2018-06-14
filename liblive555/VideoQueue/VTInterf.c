/*		VTInterf.c
视频传输模块连接视频处理模块和通讯模块中的视频数据传输模块，
采用共享公用缓冲池的方式实现两个视频模块与传输模块之间的数据传输

作者：wlw
时间：2011.8.15	
*/

#include <stdio.h>
#include <pthread.h>
#include "../typedef.h"
#include "VTInterf.h"


//视频传输接口模块初始化
//输入参数：
//		MemPool – 已经分配的视频传输接口缓冲区
//返回值：
//		-1 – 失败
//		>=0 – 成功

int32 VTInterf_Init(SVTInterf * pVtInterf, SMemory *MemPool)
{
	pthread_mutex_init(&(pVtInterf -> VT_mut),NULL);
	//视频缓冲池初始化
	if(SVideoPool_Init(&(pVtInterf -> video_pool), MemPool -> pMem, MemPool -> iSize) < 0)
	{
		printf("%p - %u\n", MemPool->pMem, MemPool->iSize);
		return -1;
	}
	SMPQueue_Init(&(pVtInterf -> mp_queue));
	SMemPacket_init(pVtInterf -> mp_array);
	return 0;
}
//VTInterf_PutData
//功能：向视频缓冲池中加入视频数据
//参数：
//	inBuf – 输入缓冲区
//	inSize – 输入数据长度
//返回值： -1 – 失败
//		  >0 – 成功

int32 VTInterf_PutData(SVTInterf * pVtInterf, frame_info * frame)
{
	pthread_mutex_lock(&(pVtInterf ->VT_mut));
	SMemPacket * ptmp = SMemPacket_New(pVtInterf ->mp_array, frame -> type, frame -> iFrameNub, frame -> iTime);
	if(ptmp == NULL)
		goto exit;
	//printf("1111111111111111111\n");
	ptmp -> memptr = SVideoPool_GetBuf(&(pVtInterf -> video_pool), frame -> length);
	if(ptmp -> memptr == NULL)
		goto exit;
	//printf("2222222222222222222\n");
	ptmp -> memsize = frame -> length;
	
	SVideoPool_Write(&(pVtInterf -> video_pool), ptmp -> memptr, frame -> pData, frame -> length);
	if(SMPQueue_Push(&(pVtInterf -> mp_queue), ptmp) < 0)
		goto exit;
//	printf("333333333333333333333\n");
	pthread_mutex_unlock(&(pVtInterf ->VT_mut));
	
	return 0;
exit:
	pthread_mutex_unlock(&(pVtInterf ->VT_mut));
	return -1;
}


//函数说明：
//		从视频缓冲池中取出视频数据
//输出参数：
//		outBuf – 输出缓冲区指针0
//返回值：
//		-1 – 失败
//	> 0 – 实际取得的数据长度

int32 VTInterf_GetData(SVTInterf * pVtInterf, frame_info * frame)
{
	pthread_mutex_lock(&(pVtInterf ->VT_mut));
	SMemPacket * ptmp = SMPQueue_Pop(&(pVtInterf -> mp_queue));
	if(ptmp == NULL)
		goto exit;
	
	frame -> length = ptmp -> memsize;
	frame -> type = ptmp -> type;
	 frame -> iTime = ptmp -> iTime;
	frame -> iFrameNub = ptmp -> iFrameNub;
	if(SVideoPool_Read(&(pVtInterf -> video_pool), frame -> pData, ptmp -> memptr, frame -> length) < 0)
		goto exit;
	if(SVideoPool_FreeBuf(&(pVtInterf -> video_pool), ptmp -> memptr, ptmp -> memsize) < 0)
		goto exit;
	if(SMemPacket_Delete(ptmp) < 0)
		goto exit;
	pthread_mutex_unlock(&(pVtInterf ->VT_mut));
	return frame -> length;
exit:
	pthread_mutex_unlock(&(pVtInterf ->VT_mut));
	return -1;
}
//丢弃所有帧数据
//返回值：
//		-1 – 失败
//		> 0  成功
int32 VTInterf_DelPFream(SVTInterf * pVtInterf)
{
	SMemPacket * ptmp;
	pthread_mutex_lock(&(pVtInterf ->VT_mut));
	while(1)
	{
		ptmp = SMPQueue_Pop(&(pVtInterf -> mp_queue));
		if(ptmp == NULL)
			break;
		if(SVideoPool_FreeBuf(&(pVtInterf -> video_pool), ptmp -> memptr, ptmp -> memsize) < 0)
			goto exit;
		SMemPacket_Delete(ptmp);
	}
	pthread_mutex_unlock(&(pVtInterf ->VT_mut));
	return 0;
exit:
	pthread_mutex_unlock(&(pVtInterf ->VT_mut));
	return -1;
}
int32 VTInterf_DelToIFrame(SVTInterf * pVtInterf)
{
	SMemPacket * ptmp;
	pthread_mutex_lock(&(pVtInterf ->VT_mut));
	ptmp = SMPQueue_Pop(&(pVtInterf -> mp_queue));
	return 0;
}
//丢弃1帧数据
//返回值：
//		-1 – 失败
//		> 0  成功
int32 VTInterf_DelOneFream(SVTInterf * pVtInterf)
{
	SMemPacket * ptmp;
	pthread_mutex_lock(&(pVtInterf ->VT_mut));
	
	ptmp = SMPQueue_Pop(&(pVtInterf -> mp_queue));
	if(ptmp == NULL)
		goto exit;
	if(SVideoPool_FreeBuf(&(pVtInterf -> video_pool), ptmp -> memptr, ptmp -> memsize) < 0)
		goto exit;
	SMemPacket_Delete(ptmp);
	
	pthread_mutex_unlock(&(pVtInterf ->VT_mut));
	return 0;
exit:
	pthread_mutex_unlock(&(pVtInterf ->VT_mut));
	return -1;
}
