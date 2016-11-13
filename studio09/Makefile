.PHONY: all clean

all: busyloop busyloop_rt busyloop_rtfork

busyloop: busyloop.c
	gcc busyloop.c -O0 -o busyloop

busyloop_rt: busyloop_rt.c
	gcc busyloop_rt.c -O0 -o busyloop_rt 

busyloop_rtfork: busyloop_rtfork.c
	gcc busyloop_rtfork.c -O0 -o busyloop_rtfork

clean:
	rm -rf busyloop busyloop_rt busyloop_rtfork
