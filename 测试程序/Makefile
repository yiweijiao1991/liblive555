

LD_FLAGS += -lpthread -lstdc++ 

COMPILE.c =  arm-none-linux-gnueabi-gcc  -c
LINK.c = arm-none-linux-gnueabi-gcc $(LD_FLAGS)

SOURCES = $(wildcard *.c) 
HEADERS = $(wildcard *.h) 

OBJFILES = $(SOURCES:%.c=%.o)

LOCAL_LIBS = libH264.a

LIBS =	$(LOCAL_LIBS) 
Target = H264

all:  $(OBJFILES)
	$(LINK.c) -o $(Target) $(OBJFILES) $(LIBS) $(LD_FLAGS)
$(OBJFILES):	%.o: %.c
	@echo Compiling $@ from $<..
	$(COMPILE.c) $(SOURCES)  

clean:
	@echo Removing 
	$(RM) -rf $(Target)
	$(RM) -rf  *.o
