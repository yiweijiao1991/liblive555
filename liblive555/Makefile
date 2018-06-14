include ./Rules.make
COMPILE.c = $(CROSS)gcc  -c
AR = $(CROSS)ar
SOURCES = $(wildcard *.c) 
HEADERS = $(wildcard *.h) 

OBJFILES = $(SOURCES:%.c=%.o)

STREAMER_OBJS = rtsp/H264_rtsp/rtsp_server.o \
		rtsp/H264_rtsp/H264VideoFileServerMediaSubsession.o \
		rtsp/H264_rtsp/MyH264VideoStreamFramer.o \
		rtsp/H264_rtsp/MyH264VideoRTPSink.o \
		rtsp/H264_rtsp/H264StreamParser.o \
		rtsp/H264_rtsp/MyJPEGVideoSource.o \
		rtsp/H264_rtsp/MyJPEGVideoRTPSink.o \
		rtsp/H264_rtsp/MJPEGVideoFileServerMediaSubsession.o 

OBJVQUE = VideoQueue/MemPacket.o\
		     VideoQueue/MPQueue.o\
		     VideoQueue/VideoPool.o\
		     VideoQueue/VTInterf.o\
			
VDESOURCE = $(OBJVQUE:%.o=%.c)
		
INCLUDES = -I rtsp/UsageEnvironment/include -I rtsp/groupsock/include \
		-I rtsp/liveMedia/include -I rtsp/BasicUsageEnvironment/include
#INCLUDE_DIR=/opt/mv_pro_4.0.1/montavista/pro/devkit/lsp/ti-davinci/linux-2.6.10_mvl401_LSP_01_30_00_082/include


USAGE_ENVIRONMENT_DIR = rtsp/UsageEnvironment
#USAGE_ENVIRONMENT_LIB = $(USAGE_ENVIRONMENT_DIR)/libUsageEnvironment.a
USAGE_ENVIRONMENT_LIB = $(USAGE_ENVIRONMENT_DIR)/*.o
BASIC_USAGE_ENVIRONMENT_DIR = rtsp/BasicUsageEnvironment
#BASIC_USAGE_ENVIRONMENT_LIB = $(BASIC_USAGE_ENVIRONMENT_DIR)/BasicUsageEnvironment.a
BASIC_USAGE_ENVIRONMENT_LIB = $(BASIC_USAGE_ENVIRONMENT_DIR)/*.o
LIVEMEDIA_DIR = rtsp/liveMedia
#LIVEMEDIA_LIB = $(LIVEMEDIA_DIR)/liveMedia.a
LIVEMEDIA_LIB = $(LIVEMEDIA_DIR)/*.o
GROUPSOCK_DIR = rtsp/groupsock
#GROUPSOCK_LIB = $(GROUPSOCK_DIR)/groupsock.a
GROUPSOCK_LIB = $(GROUPSOCK_DIR)/*.o
LOCAL_LIBS = $(LIVEMEDIA_LIB) $(GROUPSOCK_LIB) \
			$(BASIC_USAGE_ENVIRONMENT_LIB) \
			$(USAGE_ENVIRONMENT_LIB)
LIBS =	$(LOCAL_LIBS) 
Target = libH264.a

all:  $(OBJFILES)
	$(AR) -r $(Target) $(OBJFILES) $(STREAMER_OBJS) $(OBJVQUE)  $(LIBS)
$(OBJFILES):	%.o: %.c
	@echo Compiling $@ from $<..
	$(COMPILE.c) $(SOURCES)  
	cd ./rtsp/H264_rtsp/;$(MAKE);cd -
#	$(COMPILE.c) $(VDESOURCE) 
	cd ./VideoQueue/;$(MAKE);cd -
	cd ./rtsp/;$(MAKE);cd -
clean:
	@echo Removing generated files..
	$(RM) -rf $(Target)
	$(RM) -rf rtsp/H264_rtsp/*.o
	$(RM) -rf  *.o
	$(RM) -rf VideoQueue/*.o
	cd ./rtsp/;$(MAKE) clean;cd -
aclean:
	$(MAKE) -C rtsp $@

