/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// "liveMedia"
// Copyright (c) 1996-2010 Live Networks, Inc.  All rights reserved.
// A 'ServerMediaSubsession' object that creates new, unicast, "RTPSink"s
// on demand, from a MPEG-4 video file.
// Implementation
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "H264VideoFileServerMediaSubsession.hh"
#include "MyH264VideoRTPSink.hh"
#include "ByteStreamFileSource.hh"
#include "MyH264VideoStreamFramer.hh"
#include "MyJPEGVideoRTPSink.hh"
#include "MyJPEGVideoSource.hh"

H264VideoFileServerMediaSubsession*
H264VideoFileServerMediaSubsession::createNew(UsageEnvironment& env,
					       char const* fileName,
					       Boolean reuseFirstSource) {
  return new H264VideoFileServerMediaSubsession(env, fileName, reuseFirstSource);
}

H264VideoFileServerMediaSubsession
::H264VideoFileServerMediaSubsession(UsageEnvironment& env,
                                      char const* fileName, Boolean reuseFirstSource)
  : FileServerMediaSubsession(env, fileName, reuseFirstSource),
    fDoneFlag(0) ,
    fEncType(0){
//    setServerAddressAndPortForSDP(0, 50000);
}

H264VideoFileServerMediaSubsession
::~H264VideoFileServerMediaSubsession() {
}

static void afterPlayingDummy(void* clientData) {
  H264VideoFileServerMediaSubsession* subsess
    = (H264VideoFileServerMediaSubsession*)clientData;
  subsess->afterPlayingDummy1();
}

void H264VideoFileServerMediaSubsession::afterPlayingDummy1() {
  // Unschedule any pending 'checking' task:
  envir().taskScheduler().unscheduleDelayedTask(nextTask());
  // Signal the event loop that we're done:
  setDoneFlag();
}

static void checkForAuxSDPLine(void* clientData) {
  H264VideoFileServerMediaSubsession* subsess
    = (H264VideoFileServerMediaSubsession*)clientData;
  subsess->checkForAuxSDPLine1();
}

void H264VideoFileServerMediaSubsession::checkForAuxSDPLine1() {
  if (fDummyRTPSink->auxSDPLine() != NULL) {
    // Signal the event loop that we're done:
    setDoneFlag();
  } else {
    // try again after a brief delay:
    int uSecsToDelay = 100000; // 100 ms
    nextTask() = envir().taskScheduler().scheduleDelayedTask(uSecsToDelay,
			      (TaskFunc*)checkForAuxSDPLine, this);
  }
}

char const* H264VideoFileServerMediaSubsession
::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) {
  // Note: For MPEG-4 video files, the 'config' information isn't known
  // until we start reading the file.  This means that "rtpSink"s
  // "auxSDPLine()" will be NULL initially, and we need to start reading
  // data from our file until this changes.
  fDummyRTPSink = rtpSink;

  // Start reading the file:
  fDummyRTPSink->startPlaying(*inputSource, afterPlayingDummy, this);

  // Check whether the sink's 'auxSDPLine()' is ready:
  checkForAuxSDPLine(this);

  envir().taskScheduler().doEventLoop(&fDoneFlag);

  char const* auxSDPLine = fDummyRTPSink->auxSDPLine();
  return auxSDPLine;
}
enum {
	IAV_ENCODE_NONE,	// none
	IAV_ENCODE_H264,	// H.264
	IAV_ENCODE_MJPEG,	// MJPEG
};
FramedSource* H264VideoFileServerMediaSubsession
::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) {
  estBitrate = 500; // 500 kbps, estimate
	int streamId = -1;
	if (strncmp(fFileName, "live_stream1", 12) == 0) {	
		streamId = 0;
	} else if (strncmp(fFileName, "live_stream2", 12) == 0) {	
		streamId = 1;
	} else if (strncmp(fFileName, "live_stream3", 12) == 0) {	
		streamId = 2;
	} else if (strncmp(fFileName, "live_stream4", 12) == 0) {	
		streamId = 3;
	} else {
		// Create the video source:
		ByteStreamFileSource* fileSource
		  = ByteStreamFileSource::createNew(envir(), fFileName);
		if (fileSource == NULL) return NULL;
		fFileSize = fileSource->fileSize();
		
		// Create a framer for the Video Elementary Stream:
		if (fEncType == IAV_ENCODE_H264) {
			return MyH264VideoStreamFramer::createNew(envir(), fileSource);
		} else if(fEncType == IAV_ENCODE_MJPEG) {
			return NULL; //not realized
		} else {
			return NULL;
		}
	}

	if (fEncType == IAV_ENCODE_H264) {
		return MyH264VideoStreamFramer::createNew(envir(), streamId);
	} else if (fEncType == IAV_ENCODE_MJPEG) {
		int jpegQuality = getJpegQ(streamId);
		if ( jpegQuality < 0){
			return NULL;
		}
		return MyJPEGVideoSource::createNew(envir(), streamId, jpegQuality);
	} else {
		return NULL;
	}
}

RTPSink* H264VideoFileServerMediaSubsession
::createNewRTPSink(Groupsock* rtpGroupsock,
		   unsigned char rtpPayloadTypeIfDynamic,
		   FramedSource* /*inputSource*/) {
	if (fEncType == IAV_ENCODE_H264)
		return MyH264VideoRTPSink::createNew(envir(), rtpGroupsock,96 ,0X42,"h264");
	else if (fEncType == IAV_ENCODE_MJPEG)
		return MyJPEGVideoRTPSink::createNew(envir(), rtpGroupsock);
	else
		return NULL;
}


char const*
H264VideoFileServerMediaSubsession::sdpLines() {
	int encode_type = getEncType();
	if (encode_type == -1) {
		return NULL;
	}
	if (fSDPLines == NULL || fEncType != encode_type) {
		fEncType = encode_type;
		// We need to construct a set of SDP lines that describe this
		// subsession (as a unicast stream).  To do so, we first create
		// dummy (unused) source and "RTPSink" objects,
		// whose parameters we use for the SDP lines:
		unsigned estBitrate;
		FramedSource* inputSource = createNewStreamSource(0, estBitrate);
		if (inputSource == NULL) return NULL; // file not found

		struct in_addr dummyAddr;
		dummyAddr.s_addr = 0;
		Groupsock dummyGroupsock(envir(), dummyAddr, 0, 0);
		unsigned char rtpPayloadType = 96 + trackNumber()-1; // if dynamic
		RTPSink* dummyRTPSink
			= createNewRTPSink(&dummyGroupsock, rtpPayloadType, inputSource);
		setSDPLinesFromRTPSink(dummyRTPSink, inputSource, estBitrate);
		Medium::close(dummyRTPSink);
		closeStreamSource(inputSource);
	}
	return fSDPLines;
}

int H264VideoFileServerMediaSubsession::getEncType()
{
	return 1;
}

int H264VideoFileServerMediaSubsession::getJpegQ( int streamID)
{
	
	return 50;
}

