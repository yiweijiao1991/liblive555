#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "MyH264VideoStreamFramer.hh"
#include "H264StreamParser.hh"
#include "assert.h"


MyH264VideoStreamFramer*
MyH264VideoStreamFramer::createNew(UsageEnvironment & env, FramedSource * inputSource)
{
	return new MyH264VideoStreamFramer(env,inputSource);
}

MyH264VideoStreamFramer*
MyH264VideoStreamFramer::createNew(UsageEnvironment & env, int streamID)
{
	return new MyH264VideoStreamFramer(env, streamID);
}

MyH264VideoStreamFramer::MyH264VideoStreamFramer(UsageEnvironment & env, FramedSource * inputSource)
: H264VideoStreamFramer(env,inputSource),fFrameRate(30),fPictureEndMarker(False)
{
	assert(inputSource);
	fParser = new H264FileStreamParser(this, inputSource);
	fBitstreamParser = NULL;
	fStreamID = -1;
}

MyH264VideoStreamFramer::MyH264VideoStreamFramer(UsageEnvironment & env, int streamID)
: H264VideoStreamFramer(env, NULL),fFrameRate(30),fPictureEndMarker(False)
{
	struct timeval s_tv1;//结构体，秒和微秒
	struct timezone s_tz1;
	assert(streamID >= 0 && streamID < 4);
	fBitstreamParser = new H264BitStreamParser(streamID);
	fParser = NULL;
	gettimeofday(&s_tv1 , &s_tz1);
 	m_time =  (s_tv1.tv_sec % 100 )*1000000 + s_tv1.tv_usec;
	fStreamID = streamID;
	m_iFirst = 0;
}

MyH264VideoStreamFramer::~MyH264VideoStreamFramer()
{
//	printf("~MyH264VideoStreamFramer\n");
	delete fBitstreamParser;
	delete fParser;
}

void MyH264VideoStreamFramer::doGetNextFrame()
{
//	printf("	MyH264VideoStreamFramer::			  StreamFramer::doGetNextFrame\n");		//jay
	if (fBitstreamParser != NULL)
		fBitstreamParser->registerReadInterest(fTo, fMaxSize);
	if (fParser != NULL)
		fParser->registerReadInterest(fTo, fMaxSize);
	continueReadProcessing();
//	printf("	MyH264VideoStreamFramer::			  StreamFramer::doGetNextFrame - end\n");	//jay
}

Boolean MyH264VideoStreamFramer::isH264VideoStreamFramer() const
{
	return True;
}

Boolean MyH264VideoStreamFramer::pictureEndMarker()
{
	return fPictureEndMarker;
}

Boolean MyH264VideoStreamFramer::currentNALUnitEndsAccessUnit()
{
 	return True;
}
char* MyH264VideoStreamFramer:: getSPS()
{
	if (fParser != NULL)
		return fParser->getParsersps();
	else
		return fBitstreamParser->getParsersps();
}
char* MyH264VideoStreamFramer::getPPS()
{
	if (fParser != NULL)
		return fParser->getParserpps();
	else
		return fBitstreamParser->getParserpps();
}
unsigned int MyH264VideoStreamFramer::getProfileLevelID()
{
	if (fParser != NULL)
		return fParser->getPreID();
	else
		return fBitstreamParser->getPreID();
}

int MyH264VideoStreamFramer::getStreamID() 
{
	return fStreamID;
}

void MyH264VideoStreamFramer::continueReadProcessing(void* clientData,unsigned char* /*ptr*/,unsigned/* size*/,struct timeval /*presentationTime*/)
{
	MyH264VideoStreamFramer* framer = (MyH264VideoStreamFramer*) clientData;
	framer->continueReadProcessing();
}

void MyH264VideoStreamFramer::continueReadProcessing()
{
	unsigned acquiredFrameSize;
	u_int64_t frameDuration_usec;
	int iTime;
	int ipts;
	struct timeval s_tv1;//结构体，秒和微秒
	struct timezone s_tz1;
	if (fBitstreamParser != NULL && fParser == NULL) {		

		if (fBitstreamParser->parse() < 0)
			return;
		
//		printf("nalu_type %d, size %d\n", fBitstreamParser->fNaluType, fBitstreamParser->fNaluSize);		//jay
		acquiredFrameSize = fBitstreamParser->fNaluSize;
		if(acquiredFrameSize > 0)
		{
			fFrameSize = acquiredFrameSize;
#if 0
			if (!fBitstreamParser->fIsNewSession)  
			{
				if (fBitstreamParser->fPTS < fPTS) 
				{
					//frameDuration_usec = ((1<<30) + fBitstreamParser->fPTS - fPTS) * 100 / 9;		//yzhu
						frameDuration_usec = 1000 * 200 ;
				} else {
					//frameDuration_usec = (fBitstreamParser->fPTS - fPTS) * 100 / 9;	//pts is creatmented @ 90000Hz
						frameDuration_usec = 1000 * 200 ;
				}
			}
			else
				//frameDuration_usec = 0;
				frameDuration_usec = 1000 * 200 ;
			
			fPTS = fBitstreamParser->fPTS;
#endif
		//	if (fBitstreamParser->fNaluType == 7) {		//SPS NALU
		//		gettimeofday(&fPresentationTime, NULL);		//disable PTS auto adjustment
		//	} 
			if ((fBitstreamParser->fNaluType == 1) || (fBitstreamParser->fNaluType == 5))
			{	//IP NALU
				if(m_iFirst == 0)
				{
					fPresentationTime.tv_usec = 0;	
					fPresentationTime.tv_sec = 0;		
					m_iFirst = 1;
					gettimeofday(&s_tv1 , &s_tz1);
 					m_time =  (s_tv1.tv_sec % 100 )*1000000 + s_tv1.tv_usec;
				}
				gettimeofday(&s_tv1 , &s_tz1);
 				iTime =  (s_tv1.tv_sec % 100 )*1000000 + s_tv1.tv_usec;
				ipts = iTime - m_time;
				if(ipts < 0)
					ipts = 100000;
				m_time = iTime;
				fPresentationTime.tv_usec += (long) ipts; 
			}
			
			while(fPresentationTime.tv_usec >= 1000000)
			{
				fPresentationTime.tv_usec -= 1000000;
				fPresentationTime.tv_sec ++;
			}
		//	printf("---------%d--------%d-------ss-----------\n ", fPresentationTime.tv_sec, fPresentationTime.tv_usec);
			fDurationInMicroseconds = 0;		//schedule as quick as possible after get one NALU 
#if 0	//jay
	static struct timeval prev_pts;
	printf("\npt: %d sec %d usec, diff %d\n", fPresentationTime.tv_sec, fPresentationTime.tv_usec,
		(fPresentationTime.tv_sec - prev_pts.tv_sec)*1000000 + fPresentationTime.tv_usec - prev_pts.tv_usec);	
	prev_pts = fPresentationTime;
#endif
			afterGetting(this);	
		}
	}
	else 
	{	
		frameDuration_usec = (u_int64_t)(1000000 / fFrameRate);
		acquiredFrameSize = fParser->parse();
	//	printf("nalu_type %d\n", fParser->fNaluType);		//jay
		if(acquiredFrameSize > 0)
		{
			fFrameSize = acquiredFrameSize;


			if (fParser->fNaluType == 7) {		//SPS NALU
				gettimeofday(&fPresentationTime, NULL);
			} else if (fParser->fNaluType == 9)	 {	//IP NALU
				fPresentationTime.tv_usec += (long) frameDuration_usec; 
			}

			while(fPresentationTime.tv_usec >= 1000000)
			{
				fPresentationTime.tv_usec -= 1000000;
				++fPresentationTime.tv_sec;
			}
	//		if (fParser->fNaluType == 7 || fParser->fNaluType == 1)
			if (fParser->fNaluType == 9)
				fDurationInMicroseconds = (unsigned int )frameDuration_usec;

			afterGetting(this);
		}
	}
//	printf("					  StreamFramer: afterGetting\n");		//jay
}


//++++
