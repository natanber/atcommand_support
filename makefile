#aes_make: aestest.cpp
#	${CXX} -o aestest aestest.cpp -L/usr/lib/x86_64-linux-gnu -I. -lstdc++ -lcrypto
#	gcc -o aestest aestest.cpp -L/usr/lib/x86_64-linux-gnu -I. -lstdc++ -lcrypto

CC=gcc
CXX=g++
CFLAGS=-I. 
IDIR =../include
CFLAGS=-I$(IDIR)
ODIR=obj
LDIR =../lib
USE_SOM:=1
$(warning USE_SOM = $(USE_SOM) )
$(warning LEGATO_SYSROOT = $(LEGATO_SYSROOT) )
ifeq ($(USE_SOM), 1)
	toolch=$(LEGATO_SYSROOT)
 	toolchlib=$(LEGATO_SYSROOT)
	PATH=$(WP76XX_TOOLCHAIN_DIR):$PATH
	CC=arm-poky-linux-gnueabi-gcc
	CXX=arm-poky-linux-gnueabi-g++

	I1:=-I$(toolch)/usr/include/
	L1:=-L$(toolchlib)/lib/
	IDIR =../include
	ODIR=obj
	LDIR =-$(toolchlib)/usr/lib/
	LDFLAGS=--sysroot=${toolch} 
$(warning 11 )
CFLAGS:= $(I1)
LDFLAGS += -lrt -lpthread
$(warning CFLAGS = $(CFLAGS))
$(warning LDFLAGS = $(LDFLAGS))
endif

LIBS=-lstdc++ 
$(CFLAGS)+=$(LDIR)


all: atcommand_test.cpp
	${CXX} atcommand_test.cpp -o atcommand_test $(CFLAGS) $(LDFLAGS)

.PHONY: clean

clean:
#	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 
	/bin/rm atcommand_test