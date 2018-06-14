include ../../Rules.make
EXEC = testH264.exe

CC = $(CROSS)g++
INCLUDES = -I../UsageEnvironment/include -I../groupsock/include \
		-I../liveMedia/include -I../BasicUsageEnvironment/include

STREAMER_OBJS = rtsp_server.o \
		H264VideoFileServerMediaSubsession.o \
		MyH264VideoStreamFramer.o \
		MyH264VideoRTPSink.o \
		H264StreamParser.o \
		MyJPEGVideoSource.o \
		MyJPEGVideoRTPSink.o \
		MJPEGVideoFileServerMediaSubsession.o



USAGE_ENVIRONMENT_DIR = ../UsageEnvironment
USAGE_ENVIRONMENT_LIB = $(USAGE_ENVIRONMENT_DIR)/libUsageEnvironment.a
BASIC_USAGE_ENVIRONMENT_DIR = ../BasicUsageEnvironment
BASIC_USAGE_ENVIRONMENT_LIB = $(BASIC_USAGE_ENVIRONMENT_DIR)/BasicUsageEnvironment.a
LIVEMEDIA_DIR = ../liveMedia
LIVEMEDIA_LIB = $(LIVEMEDIA_DIR)/liveMedia.a
GROUPSOCK_DIR = ../groupsock
GROUPSOCK_LIB = $(GROUPSOCK_DIR)/groupsock.a
LOCAL_LIBS = $(LIVEMEDIA_LIB) $(GROUPSOCK_LIB) \
			$(BASIC_USAGE_ENVIRONMENT_LIB) \
			$(USAGE_ENVIRONMENT_LIB)

LINK_EXTRA_LIBS = -lpthread
LIBS =	$(LOCAL_LIBS) $(LINK_EXTRA_LIBS)
##################### End of Included LIBS:
		

##################### Executable apps:
$(EXEC): $(STREAMER_OBJS)
#	@$(CC) -Wall -o $(EXEC) $(STREAMER_OBJS)  $(LIBS)
%.o: %.cpp
	@$(CC) -Wall -c $(INCLUDES) -o $@ $<

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o *.*~ *.out *.exe *.a



