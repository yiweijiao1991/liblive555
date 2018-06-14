#include "FrameH264.h"
#include <stdio.h>
#include <malloc.h>
#include "myH264.h"
int main()
{	
	char *data = NULL;
	int len = 0;
	data = (char *)malloc(1024*1024);
	Init_myH264();
	//测试时使用，初始化从文件读取H264视频流数据
	InitGetFrame();
	while(1)
	{
		//获取一帧图像
		len = getFrame(data);
		PU_H264data(data,len);
	}


}

