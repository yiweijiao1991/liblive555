#ifndef _QUEUEFRAME_H
#define _QUEUEFRAME_H
#include "VideoQueue/VTInterf.h"
#include "Mem.h"
/*
函数名称：initqueueFrame
函数功能:初始化视频入队模块
参数：frame[in]需要被初始化的帧结构
返回值： 无
author： yiwj
*/
//void initqueueFrame(frame_info *frame);
void  initqueueFrame(SMemory *m_EncMem,frame_info *frame);
/*
函数名称：queueFrame
函数功能：把一帧图像入队
参数：frame[in]需要入队的帧结构
返回值： 无
author：yiwj
*/
void queueFrame(frame_info *frame);
void MJPEGqueueFrame(frame_info *frame);

#endif
