CFLAGS=-I../include -O 
LIBDIR=../lib
default	: bardecode.o
	cc -s bardecode.o -o bardecode ${LIBDIR}/bardecode.a -ltiff -ljpeg -lz -lm

shared : bardecode.o
	cc -s bardecode.o -o bardecode -L${LIBDIR} -lbardecode -ltiff -ljpeg -lz -lm

clean:
	rm -f bardecode bardecode.o
