include ../../Rules.make
EXEC = BasicUsageEnvironment.a
SRC = $(wildcard *.cpp)
File = $(notdir $(SRC))
OBJECT = $(patsubst %.cpp, %.o, $(File))
CC = $(CROSS)g++
LIBA = $(CROSS)ar r
INCLUDES = -Iinclude -I../UsageEnvironment/include -I../groupsock/include -I. -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
LINKFLAG =-O2 -Wall 
all:$(OBJECT)
	@$(LIBA)  $(EXEC) $(OBJECT)

%.o: %.cpp
	@$(CC)  $(LINKFLAG) -c $(INCLUDES) -o $@ $<
clean:
	-rm -f $(EXEC) *.elf *.gdb *.o *.*~ *.out *.exe *.a

