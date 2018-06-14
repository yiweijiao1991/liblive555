#ifndef __MY_H264__
#define __MY_H264__
/*
函数名称：Init_myH264();
函数功能： 初始化H264模块
输入参数： 无
输出参数： 无
返回值：>0 成功 <0 失败
*/
int Init_myH264();
/*
函数名称： PU_H264data(char *data,int len);
函数功能：向H264模块传入一帧数据
输入参数： data 帧数据的指针 len帧数据的长度
输出参数：无
返回值：1
*/
int PU_H264data(char *data,int len);
#endif