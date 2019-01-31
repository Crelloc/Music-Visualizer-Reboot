CC = gcc
CFLAGS=`pkg-config --cflags fftw3 sdl2`
CFLAGS += -Wall -Wextra -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs

ifeq ($(CC),gcc)
    CFLAGS += -Wjump-misses-init -Wlogical-op
endif

CFLAGS += -O2 -g -ggdb -std=c99 -fbuiltin

LDFLAGS=`pkg-config --libs fftw3 sdl2`
LDFLAGS += -lm

visual.exe : visualize.o dataprocessing.o audioInformation.o
	$(CC) -o $@ $^ $(LDFLAGS)

visualize.o : visualize.c dataprocessing.h audioInformation.h
	$(CC) $(CFLAGS) -c $*.c  

dataprocessing.o : dataprocessing.c dataprocessing.h
	$(CC) $(CFLAGS) -c $*.c 

audioInformation.o: audioInformation.c audioInformation.h
	$(CC) $(CFLAGS) -c $<  

clean:
	rm -rf *.o visual.exe

