##
# OVP demo makefile adapted for this platform and
# the application.
#
# Author: Shahzeb Ihsan
# Version: 1.0
##

IMPERAS_HOME := $(shell getpath.exe "$(IMPERAS_HOME)")

# Build locally without using a VLNV library
NOVLNV=1

# Build using two passes so that each pass includes a different Makefile
MAKEPASS?=0
ifeq ($(MAKEPASS),0)
all:
	$(MAKE) MAKEPASS=1
	$(MAKE) MAKEPASS=2
clean::
	- rm -f *.elf *.exe *.o *.lst *.map *.dll
endif

# Pass 1, build the Platform
ifeq ($(MAKEPASS),1)

SRC=cortex_m3_test_platform.c
include $(IMPERAS_HOME)/ImperasLib/source/buildutils/Makefile.platform
endif

# Pass 2, build the application
ifeq ($(MAKEPASS),2)

OPTIMISATION?=-O0

CROSS=ARM_CORTEX_M3
SRC=cortex_m3_test_app.c
OBJECTS=start.o cortex_m3_test_app.o
APP=$(patsubst %.c,%.elf,$(SRC))
include $(IMPERAS_HOME)/bin/Makefile.include
include $(IMPERAS_LIB)/CrossCompiler/$(CROSS).makefile.include

all: $(APP)

start.o: start.s
	$(V) echo "Compiling $<"
	$(V) $(IMPERAS_AS) -ahls -mapcs-32 -o $@ $< > asm.lst
		
cortex_m3_test_app.o: cortex_m3_test_app.c
	$(V) echo "Compiling $<"
	$(V) $(IMPERAS_CC) -Wa,-ahls, -L -g -ggdb -c -O0 -o $@ $< $(OPTIMISATION) > c.lst 
	   
$(APP): $(OBJECTS)
	$(V) echo "Linking $^"
	$(V) $(IMPERAS_LD) -o $@ -Tlink.ld -N -n $^ -Map mem.map
	$(V) echo "Done...deleting object files"
	$(V) rm -f *.o
	$(V) echo "Generating object dump file..."
	$(V) $(IMPERAS_OBJDUMP) -s -d -h -x -a -f -p -t $(APP) > symbols.lst
endif
