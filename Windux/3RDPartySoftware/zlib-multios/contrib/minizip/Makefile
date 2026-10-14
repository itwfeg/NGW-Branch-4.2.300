
HOBART_APP ?= $(HOME)/projects/Hobart/shared_code/Application

include $(HOBART_APP)/hobartarch.mk

CC=$(CROSS_COMPILE)gcc
CFLAGS=$(FFLAGS) -I../..

AR=$(CROSS_COMPILE)ar

UNZ_OBJS = miniunz.o unzip.o ioapi.o ../../libz.a
ZIP_OBJS = minizip.o zip.o   ioapi.o ../../libz.a

.c.o:
	$(CC) -c $(CFLAGS) $*.c

all: install

all_old: miniunz minizip

miniunz:  $(UNZ_OBJS)
	$(CC) $(CFLAGS) -o $@ $(UNZ_OBJS)

minizip:  $(ZIP_OBJS)
	$(CC) $(CFLAGS) -o $@ $(ZIP_OBJS)

test:	miniunz minizip
	./minizip test readme.txt
	./miniunz -l test.zip
	mv readme.txt readme.old
	./miniunz test.zip

clean:
	/bin/rm -f *.o *~ minizip miniunz minizip.a

lib: $(UNZ_OBJS) $(ZIP_OBJS)
	$(AR) -r -c minizip.a *.o

install: lib
	cp minizip.a ../../../../Application/output/lib/
