include ../../Rules.make
EXEC = libUsageEnvironment.a
SRC1 = $(wildcard *.cpp)
File1 = $(notdir $(SRC1))
OBJECT1 = $(patsubst %.cpp, %.o, $(File1))
SRC2 = $(wildcard *.c)
File2 = $(notdir $(SRC2))
OBJECT2 = $(patsubst %.c, %.o, $(File2))
OBJECT = $(OBJECT1) $(OBJECT2)
CC = $(CROSS)gcc
LIBA = $(CROSS)ar r
INCLUDES = -Iinclude -I../UsageEnvironment/include -I../groupsock/include -I. -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
LINKFLAG =-O2 -Wall
all:$(OBJECT)
	@$(LIBA)  $(EXEC) $(OBJECT)

%.o: %.cpp
	@$(CC)  $(LINKFLAG) -c $(INCLUDES) -o $@ $<
%.o: %.c
	@$(CC)  $(LINKFLAG) -c $(INCLUDES) -o $@ $<
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o *.*~ *.out *.exe *.a

