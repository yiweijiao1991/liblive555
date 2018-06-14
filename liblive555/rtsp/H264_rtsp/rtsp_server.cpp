#include <stdio.h>
#include <getopt.h>
#include <sched.h>
#include <pthread.h>
#include <signal.h>
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"
#include "MyH264VideoStreamFramer.hh"
#include "MyH264VideoRTPSink.hh"
#include "H264VideoFileServerMediaSubsession.hh"
#include "MyJPEGVideoSource.hh"
#include "MyJPEGVideoRTPSink.hh"
#include "MJPEGVideoFileServerMediaSubsession.hh"
#include <fcntl.h>
#include <sys/ioctl.h>

UsageEnvironment* env;


char const* descriptionString
   = "Session streamed by \"Eparking RTSP Server\"";


static portNumBits clientPort;
static Boolean is_ssm = False;

// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
Boolean reuseFirstSource = True;

void setup_stream1(RTSPServer* rtspServer)
{
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	UsageEnvironment* env_stream1 = BasicUsageEnvironment::createNew(*scheduler);
	
	 // Set up each of the possible streams that can be served by the
	 // RTSP server.  Each such stream is implemented using a
	 // "ServerMediaSession" object, plus one or more
	 // "ServerMediaSubsession" objects for each audio/video substream.
	
	char const* streamName = "stream1";
	char const* inputFileName = "live_stream1";

	ServerMediaSession* sms
	 	= ServerMediaSession::createNew(*env, streamName, streamName,
									 descriptionString, is_ssm /*SSM*/);

	ServerMediaSubsession* subsession 
		= H264VideoFileServerMediaSubsession::createNew(*env_stream1, 
			inputFileName, reuseFirstSource);

	sms->addSubsession(subsession);
	subsession->setServerAddressAndPortForSDP(0, clientPort);
	
	rtspServer->addServerMediaSession(sms);

	char* url = rtspServer->rtspURL(sms);
	*env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;



}

static portNumBits clientPort2;
static Boolean is_ssm2 = False;

void setup_stream2(RTSPServer* rtspServer)
{
	
	TaskScheduler* scheduler2 = BasicTaskScheduler::createNew();
	UsageEnvironment* env_stream2 = BasicUsageEnvironment::createNew(*scheduler2);

	char const* streamName2 = "mjpegstream";
	char const* inputFileName2 = "live_stream2";

	ServerMediaSession* sms2
	 	= ServerMediaSession::createNew(*env, streamName2, streamName2,
									 descriptionString, is_ssm2 /*SSM*/);
	
	ServerMediaSubsession* subsession2 
		= MJPEGVideoFileServerMediaSubsession::createNew(*env_stream2, 
			inputFileName2, reuseFirstSource);

	sms2->addSubsession(subsession2);
	subsession2 ->setServerAddressAndPortForSDP(0, clientPort2);

	rtspServer->addServerMediaSession(sms2);

	char* url2 = rtspServer->rtspURL(sms2);
	*env << "Play this stream using the URL \"" << url2 << "\"\n";
	delete[] url2;
}


RTSPServer* rtspServer;
extern "C" void *  StartRtspServer(void * arg) 
{
	printf("**********    rtsp_thread start  *************\n");
	// Begin by setting up our usage environment:
	TaskScheduler* scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
	// To implement client access control to the RTSP server, do the following:
	authDB = new UserAuthenticationDatabase;
	authDB->addUserRecord("username1", "password1"); // replace these with real strings
	// Repeat the above with each <username>, <password> that you wish to allow
	// access to the server.
#endif

	// Create the RTSP server:
	 rtspServer = RTSPServer::createNew(*env, 554, authDB);
	if (rtspServer == NULL) {
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}


	setup_stream1(rtspServer);
	setup_stream2(rtspServer);

	env->taskScheduler().doEventLoop(); // does not return
	
	return 0; // only to prevent compiler warning

}

 extern "C" void c1loseSocket()
{
	
	close(rtspServer -> fServerSocket);
}


