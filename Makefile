CC = gcc
CFLAGS += -Wall -Wextra -Wpedantic \
          -Wformat=2 -Wno-unused-parameter -Wshadow \
          -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
          -Wredundant-decls -Wnested-externs -Wmissing-include-dirs

ifeq ($(CC),gcc)
    CFLAGS += -Wjump-misses-init -Wlogical-op
endif

CFLAGS += -O2 -g -std=c11

LDFLAGS = -lm

LDFLAGS += -L/usr/local/lib -lfftw3 -lSDL2

visual.exe : visualize.o dataprocessing.o audioInformation.o
	$(CC) -o $@ $^ $(LDFLAGS)

visualize.o : visualize.c dataprocessing.h audioInformation.h
	$(CC) $(CFLAGS) -c visualize.c  

dataprocessing.o : dataprocessing.c dataprocessing.h
	$(CC) $(CFLAGS) -c dataprocessing.c 

audioInformation.o: audioInformation.c audioInformation.h
	$(CC) $(CFLAGS) -c audioInformation.c  

clean:
	rm -rf *.o visual.exe

