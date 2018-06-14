#ifndef _MY_H264_VIDEO_FILE_SINK_HH
#define _MY_H264_VIDEO_FILE_SINK_HH

#include "H264VideoFileSink.hh"

class MyH264VideoFileSink: public H264VideoFileSink 
{
public:
	MyH264VideoFileSink(UsageEnvironment& env, FILE* fid, unsigned bufferSize,
			 char const* perFrameFileNamePrefix);
  	static MyH264VideoFileSink* createNew(UsageEnvironment& env, char const* fileName,
						 unsigned bufferSize = 10000,
						 Boolean oneFilePerFrame = False);

  	virtual void afterGettingFrame1(unsigned frameSize,
				  struct timeval presentationTime);
	 virtual ~MyH264VideoFileSink();
};

#endif
