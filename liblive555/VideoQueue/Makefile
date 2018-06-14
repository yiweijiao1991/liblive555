include ../Rules.make
C_FLAGS += -O2 -w
COMPILE.c = $(CROSS)gcc  -c
SOURCES = $(wildcard *.c) 
HEADERS = $(wildcard *.h) 

OBJFILES = $(SOURCES:%.c=%.o)



all:  $(OBJFILES)
#	@echo $^..
#	@echo Linking $@ from $^..
#	@$(MAKE)  -C rtsp all
	
#	@$(LINK.c) -ldl -o $@ $^ $(LIBS) 
	$(COMPILE.c) $(OBJFILES) 
 

clean:
	@echo Removing generated files..
	$(RM) -rf  *.o


