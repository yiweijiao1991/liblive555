/*
 * timeStatistics.h
 *
 *  Created on: 2014-1-20
 *      Author: root
 */

#ifndef TIMESTATISTICS_H_
#define TIMESTATISTICS_H_

typedef struct _S_TimeStat{
	unsigned int timeVal;//两帧间耗时
	unsigned int frameNums;
	unsigned int statInterVal[20];//0~10 20个区间
}S_TimeStat;

#endif /* TIMESTATISTICS_H_ */
