#include <sys/time.h>
#include "typedef.h"
#include "Mem.h"
#include "VideoQueue/VTInterf.h"
extern SMemory *m_EncMem;         //压缩输出缓冲区
extern SVTInterf g_VtInterf;  //队列

static int64 beginTime = 0; //开始时间
static int64 getTime; //当前时间

/*
函数名称：initqueueFrame
函数功能:初始化视频入队模块
参数：frame[in]需要被初始化的帧结构
返回值： 无
author： yiwj
*/

void initqueueFrame( SMemory *m_EncMem,frame_info *frame)
{
	struct timeval s_tv1;//结构体，秒和微秒
	struct timezone s_tz1;

	frame->pData =m_EncMem->pMem;// 为帧结构体获取缓冲空间
	//获取开始时间
	gettimeofday(&s_tv1 , &s_tz1);
	beginTime = s_tv1.tv_sec*1000000 + s_tv1.tv_usec;
}
/*
函数名称：queueFrame
函数功能：把一帧图像入队
参数：frame[in]需要入队的帧结构
返回值： 无
author：yiwj
*/
void queueFrame(frame_info *frame)
{
	struct timeval s_tv1;//结构体，秒和微秒
	struct timezone s_tz1;
	//获取当前时间
	gettimeofday(&s_tv1 , &s_tz1);
	getTime = s_tv1.tv_sec*1000000 + s_tv1.tv_usec;
	//获取一帧持续时间
	frame->iTime = getTime -beginTime;
	beginTime = getTime;
	//帧类型：默认0保留
	frame->type = 0;


	// 帧结构入队
	if(VTInterf_PutData(&g_VtInterf, frame) < 0)
	{
			//如果入队失败，清空队列之前的数据
			VTInterf_DelPFream(&g_VtInterf);
			//再次入队
			VTInterf_PutData(&g_VtInterf, frame);
	}
}


/********************************************************/


extern SVTInterf g_VtInterf2;  //队列
/*
函数名称：queueFrame
函数功能：把一帧图像入队
参数：frame[in]需要入队的帧结构
返回值： 无
author：yiwj
*/
void MJPEGqueueFrame(frame_info *frame)
{
	
	//帧类型：默认0保留
	frame->type = 0;

	// 帧结构入队
	if(VTInterf_PutData(&g_VtInterf2, frame) < 0)
	{
			//如果入队失败，清空队列之前的数据
			VTInterf_DelPFream(&g_VtInterf2);
			//再次入队
			VTInterf_PutData(&g_VtInterf2, frame);
	}
}



