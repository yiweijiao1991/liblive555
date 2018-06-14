/*		VideoPool.h
	视频传输缓冲池处理子模块
作者：wlw
时间：2011.8.15	
*/

#ifndef _VIDEOPOOL_H
#define _VTDEOPOOL_H

typedef struct 
{
	int32	maxsize;//	缓冲池大小	0(默认)
	uint8*	memptr;//	缓冲池起始指针	NULL(默认)
	int32	readptr	;//读取偏移	0(默认)
	int32	writeptr;//	写入偏移	0(默认)
	int32	allocated;//	已分配大小	0(默认)
	int32	unused;//	未分配大小	maxsize(默认)

}SVideoPool, *pSVideoPool;

#ifdef __cplusplus
extern "C"
{
#endif


//视频传输缓冲池初始化
int32 SVideoPool_Init(SVideoPool * pVieoPool, uint8 *memPtr, int32 memSize);

//从视频传输缓冲池中分配一块内存数据
uint8* SVideoPool_GetBuf(SVideoPool * pVieoPool, int32 size);

//从视频传输缓冲池中分配一块内存数据
int  SVideoPool_FreeBuf(SVideoPool * pVieoPool, uint8 *buf, int32 size);

//向视频缓冲区中拷贝数据
int  SVideoPool_Write(SVideoPool * pVieoPool, uint8 *dst, uint8 * src, int32 size);

//从视频缓冲区中向外拷贝数据
int SVideoPool_Read(SVideoPool * pVieoPool, uint8 *dst, uint8 *src, int32 size);
#ifdef __cplusplus
}
#endif

#endif
