CFLAGS       = -fPIC -pedantic -Wall -std=gnu99
LDFLAGS      = -shared

CC           = $(CROSS_COMPILE)gcc

all: libafu050.so example

libafu050.so: afu050.c afu050.h tisuvc.h
	$(CC) $(CFLAGS) $(LDFLAGS) -O0 -g $< -o $@ `pkg-config --cflags --libs libusb-1.0` -lpthread


example: example.c libafu050.so
	$(CC) -O0 -g $< -o $@ -L. -lafu050 `pkg-config --cflags --libs libusb-1.0 gtk+-2.0 gstreamer-0.10` -lgstapp-0.10 -lgstinterfaces-0.10 -lm

rules-install: 50-euvccam.rules
	cp 50-euvccam.rules /etc/udev/rules.d

clean:
	rm -f libafu050.so
	rm -f example
