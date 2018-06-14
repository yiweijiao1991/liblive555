
/*		rtsp_server.h
	视频采集
作者：wlw
时间：2011.8.16
*/

#ifndef _RTSP_H
#define _RTSP_H

#ifdef __cplusplus
extern "C"
{
#endif
//开启rtsp服务
int  rtsp_start();
//停止rtsp服务
int rtsp_exit();
#ifdef __cplusplus
}
#endif

#endif
