#include "MyH264VideoFileSink.hh"
#include "OutputFile.hh"


int iFrame = 0;
MyH264VideoFileSink
::MyH264VideoFileSink(UsageEnvironment& env, FILE* fid, unsigned bufferSize,
			 char const* perFrameFileNamePrefix)
	: H264VideoFileSink(env, fid, bufferSize, perFrameFileNamePrefix) 
{
}

MyH264VideoFileSink::~MyH264VideoFileSink() 
{
}

MyH264VideoFileSink*
MyH264VideoFileSink::createNew(UsageEnvironment& env, char const* fileName,
				  unsigned bufferSize, Boolean oneFilePerFrame) {
  	do 
   	{
		FILE* fid;
		char const* perFrameFileNamePrefix;
		if (oneFilePerFrame) 
		{
			// Create the fid for each frame
			fid = NULL;
		 	 perFrameFileNamePrefix = fileName;
		}
		else 
		{
			// Normal case: create the fid once
		  	fid = OpenOutputFile(env, fileName);
		  	if (fid == NULL) 
				break;
		  	perFrameFileNamePrefix = NULL;
		}

	 	return new MyH264VideoFileSink(env, fid, bufferSize, perFrameFileNamePrefix);
	} while (0);

  return NULL;
}
void MyH264VideoFileSink::afterGettingFrame1(unsigned frameSize,
					  struct timeval presentationTime) 
{
 
	iFrame ++;
	if(iFrame % 30 == 0)
			printf(" ------------- %d     %x %x %x  %x  %x %x ----------- \n", iFrame, *fBuffer, *(fBuffer + 1), *(fBuffer + 2), *(fBuffer + 3), *(fBuffer + 4), *(fBuffer + 5) );
	// H264VideoFileSink::afterGettingFrame1(frameSize, presentationTime);
}
