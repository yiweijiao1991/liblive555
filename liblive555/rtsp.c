#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include "typedef.h"
#include "Mem.h"
//#include "comman.h"
#include "VideoQueue/VTInterf.h"
#include "timeStatistics.h"
#include "rtsp.h"
pthread_t	m_rtsp_server_thread_t;		//RTSP线程
int32 m_rtspFrame;
S_TimeStat g_timeStat = {0, 0, {0}};
void *  StartRtspServer(void * arg);
void c1loseSocket();

extern int m_EncodeType;
extern  SVTInterf  g_VtInterf;
extern  SVTInterf  g_VtInterf2;
static SMemory * m_RtspMem;

static SMemory * m_RtspMemMjpeg;
//-1  - 失败  0 － 成功
int32 rtsp_mem_init()
{
	//分配内存
	 m_RtspMem = SMem_New(WIDTH * HEIGHT*3, _MEM_VIRTUAL, "rtsp");
	 if(m_RtspMem == NULL)
	 {
		 return -1;
	 }
	//分配内存
	m_RtspMemMjpeg = SMem_New(WIDTH * HEIGHT*3, _MEM_VIRTUAL, "mjpeg");
	 if(m_RtspMemMjpeg== NULL)
	 {
		 return -1;
	 }
	 return 0;
}
//开启rtsp服务
int32  rtsp_thread_start()
{
	//zzh add 6s sleep, waiting h.264 compress start at 2012-12-18
//	sleep(6);	
	memset(&m_rtsp_server_thread_t, 0, sizeof(m_rtsp_server_thread_t));
	//创建线程  开启RTSP 服务
	if((pthread_create(&m_rtsp_server_thread_t, NULL, StartRtspServer, NULL)) != 0)       
	{
		printf("rtsp_client_thread failed!\n");
		return -1;
	}
	return 0;
}
//停止rtsp服务
int32 rtsp_thread_exit()
{
	int pthread_kill_err;
	c1loseSocket();
	pthread_kill_err = pthread_kill(m_rtsp_server_thread_t,0);
	if(pthread_kill_err == ESRCH) printf("thead not exists\n");
	else if(pthread_kill_err == EINVAL) printf("thead invalid\n");
	else  
	{
		printf("thead alive\n");
		pthread_cancel(m_rtsp_server_thread_t);
	}
	return 0;
}
//从队列中获取一帧数据 传送至rtsp服务线程
int32  rtsp_getFrame(frame_info * info)
{
	int len = 0;
	info ->pData =  m_RtspMem -> pMem;
	unsigned char *pFrame = NULL;
	while(1)
	{  
		//从队列中获取一帧数据
		len = VTInterf_GetData(&g_VtInterf, info);
		if(len < 0){
			usleep(20000);
		}else
		{
			if(info->type == 0)//核对帧类型
			{
				pFrame = info->pData;
				if (((pFrame[0] == 0x00) &&(pFrame[1] == 0x00)&&(pFrame[2] == 0x01)) || ((pFrame[0] == 0x00) &&(pFrame[1] == 0x00)&&(pFrame[2] == 0x00) &&(pFrame[3] == 0x01)))
				{
					return 0;
				}


				
			}

		}
		usleep(30000);
	}
	return 0;
}
//从队列中获取一帧数据 传送至rtsp服务线程
int32  rtsp_getMJPEGFrame(frame_info * info)
{
	int len = 0;
	info ->pData =  m_RtspMemMjpeg -> pMem;
	unsigned char *pFrame = NULL;
	unsigned char *pFrame_end = NULL;
	while(1)
	{  
		//从队列中获取一帧数据
		len = VTInterf_GetData(&g_VtInterf2, info);
		if(len < 0){
			usleep(20000);
		}else
		{
			if(info->type == 0)//核对帧类型
			{

				pFrame = info->pData;
				pFrame_end = info->pData + info->length-2;
				if (*pFrame_end == 0xFF && *(pFrame_end+1) ==0xD9) {
					if ((*pFrame++ == 0xFF) && (*pFrame++ == 0xD8)) {
						return 0;	
					}
				}
			
			}

		}
		usleep(30000);
	}
	return 0;
}
