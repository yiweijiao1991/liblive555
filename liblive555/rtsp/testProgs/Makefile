include ../../Rules.make
CC = $(CROSS)g++
LINKFLAG = -g -O2 -Wall 

INCLUDES = -I../UsageEnvironment/include -I../groupsock/include \
		-I../liveMedia/include -I../BasicUsageEnvironment/include

OPEN_RTSP_OBJS    = openRTSP.o playCommon.o MyH264VideoFileSink.o

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
LIBS =	$(LOCAL_LIBS)


openRTSP.exe:	$(OPEN_RTSP_OBJS) $(LOCAL_LIBS)
	@$(CC) -g -o openRTSP.exe $(OPEN_RTSP_OBJS) $(LOCAL_LIBS) -lpthread
	#cp openRTSP.exe /opt/nfs/test/

%.o: %.cpp
	$(CC)  $(LINKFLAG) -c $(INCLUDES) -o $@ $<
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o *.*~ *.out *.exe *.a
