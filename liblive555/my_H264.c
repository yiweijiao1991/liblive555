#include <string.h>
#include "typedef.h"
#include "rtsp.h"
#include "Mem.h"
#include "queueFrame.h"
#include "stdio.h"
#include "my_H264.h"
SVTInterf g_VtInterf;  //ÊÓÆµ»º³å¶ÓÁÐ
SMemory *m_TrMem;         //ÊÓÆµ·þÎñ¶ÓÁÐÄÚŽæ±àºÅ
SMemory *m_EncMem;         //ÍŒÏñÊä³ö»º³åÇø
frame_info frame;     //Ö¡ÊýŸÝ


SVTInterf g_VtInterf2;  //ÊÓÆµ»º³å¶ÓÁÐ
SMemory *m_TrMem2;         //ÊÓÆµ·þÎñ¶ÓÁÐÄÚŽæ±àºÅ
SMemory *m_EncMem2;         //ÍŒÏñÊä³ö»º³åÇø
frame_info frame2;     //Ö¡ÊýŸÝ

extern unsigned char myfQ;//JPEG 的图像质量
/*
初始化视频流
*/
int Init_myH264()
{
	//1.ÄÚŽæ·ÖÅä
	SMem_Init();
	m_TrMem   = SMem_New(MemorySize_20M, _MEM_VIRTUAL, "TrMem");      //视频服务队列内存编号
	m_EncMem  = SMem_New(MemorySize_1M, _MEM_CONTINUOUS, "EncMem");  //压缩输出缓冲区

	m_TrMem2   = SMem_New(MemorySize_20M, _MEM_VIRTUAL, "TrMem2");      //ÊÓÆµ·þÎñ¶ÓÁÐÄÚŽæ±àºÅ
	m_EncMem2  = SMem_New(MemorySize_1M, _MEM_CONTINUOUS, "EncMem2");  //Ñ¹ËõÊä³ö»º³åÇø
//2.rtspÄÚŽæ³õÊŒ»¯
	if(rtsp_mem_init() < 0)
	{
		printf("rtsp_mem_init  false\n");
		return -1;
	}
	//3.ÎªÄÚŽæ·ÖÅäŽæŽ¢¿ÕŒä
	GlobalSMemAlloc();
	//4.ÊÓÆµ»º³å¶ÓÁÐ³õÊŒ»¯
	if(VTInterf_Init(&g_VtInterf,m_TrMem) < 0)
	{
		printf("VTInterf_Init  failed \n");
		return -2;
	}
	if(VTInterf_Init(&g_VtInterf2,m_TrMem2) < 0)
	{
		printf("VTInterf_Init2  failed \n");
		return -2;
	}

	//5.Æô¶¯rtsp·þÎñÏß³Ì
	if(rtsp_thread_start() < 0)     //RTSP rtsp.c
	{
		printf("start rtsp server failed\n");
		return -3;
	}
	//6.³õÊŒ»¯Ö¡Èë¶ÓÄ£¿é
	initqueueFrame(m_EncMem,&frame);
	initqueueFrame(m_EncMem2,&frame2);

	return 1;
	
}
/*
将一帧H.264图像压入发送队列
*/
int PU_H264data(char *data,int len)
{
	//7.获取一帧图像
	if(len >0)
	{
		memcpy(frame.pData,data,len);
		frame.length = len;
		//8一帧图像入队
		queueFrame(&frame);
	}
	return 1;
}
/*
将一帧JPEG图像压入发送队列
*/
int PU_MJPEGdata(char *data,int len)
{
		
	if(len >0)
	{
		frame2.length = len;
		memcpy(frame2.pData,data,frame2.length);
		MJPEGqueueFrame(&frame2);		
	}
	return 1;
}

/*
设置JPEG的品质
*/
void  setJPEGQuality(unsigned char fQ)
{
	myfQ = fQ;
}


///////////////////////////////////////////////
/********************************************
	测试专用代码，
*********************************************/

int flag = 0;
static FILE *pFile = NULL;
static int framenum = 0;
int mjpegOpen()
{

	if(pFile == NULL)
	{
		framenum = 0;
		printf("``````````````````````````!   open \n");
		pFile =fopen("1.mjpeg","rb+");//以读的方式打开文件
		usleep(20000);
		
	}
	return 0;
}
int mjpegclose()
{
	if(pFile != NULL)
	{
		printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$   close \n");
		fclose(pFile);
		pFile = NULL;
		usleep(20000);
	}
	return 0;
}
int getOneFrame(unsigned char *buff)
{
	unsigned char lastchar = 0;
	unsigned char nowchar = 0;
	int index = 0;

	if(pFile == NULL)	
		return -1;
	if(framenum > 120)
		return -1;

	do
	{
		nowchar = fgetc(pFile);		
		if(nowchar == EOF)
			return -1;
		if(lastchar == 0xFF && nowchar == 0xD8)
			break;
		lastchar = nowchar;

	}while(1);
	buff[index++] = lastchar;
	buff[index++] = nowchar;
	
	lastchar = 0;
	nowchar = 0;
	do
	{
		nowchar = fgetc(pFile);		
		if(nowchar == EOF)
			return -1;
		if(lastchar == 0xFF && nowchar == 0xD9)
			break;
		lastchar = nowchar;
		buff[index++] = nowchar;
		if(index > 500*1024)
			return -1;
	}while(1);
	buff[index++] = nowchar;
	framenum ++;
	return index;
}
int JPEGTEST()
{
	
	unsigned char buff[1024*1024] = {0};
	int ret = 0;
	mjpegOpen();

	ret = getOneFrame(buff);

	if(ret < 0)
	{
		mjpegclose();		
	}		
	else
	{
		PU_MJPEGdata(buff,ret);
	}
	return 1;
}

