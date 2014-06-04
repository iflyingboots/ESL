######################################################################
# Choose your favorite C compiler
CC = /data/usr/local/share/codesourcery/bin/arm-none-linux-gnueabi-gcc
EXEC=klt
######################################################################
# -DNDEBUG prevents the assert() statements from being included in
# the code.  If you are having problems running the code, you might
# want to comment this line to see if an assert() statement fires.
FLAG1 = -DNDEBUG

######################################################################
# -DKLT_USE_QSORT forces the code to use the standard qsort()
# routine.  Otherwise it will use a quicksort routine that takes
# advantage of our specific data structure to greatly reduce the
# running time on some machines.  Uncomment this line if for some
# reason you are unhappy with the special routine.
FLAG2 = -DKLT_USE_QSORT

INC = -I.
LIBS = -L.
######################################################################
# Add your favorite C flags here.

CFLAGS = $(FLAG1) $(FLAG2)
CFLAGS += -O3 -Wall -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -ffast-math -fomit-frame-pointer -ftree-vectorize -ftree-vectorizer-verbose=2 -mvectorize-with-neon-quad -fsingle-precision-constant -fno-math-errno -ffinite-math-only -fno-signed-zeros -funroll-loops
LDFLAGS=-lm

######################################################################
# sources
CSRCS   =	main.c \
			convolve.c error.c pnmio.c pyramid.c selectGoodFeatures.c \
			storeFeatures.c trackFeatures.c klt.c klt_util.c writeFeatures.c

CPPSRCS =

######################################################################
# rules
OBJS=$(CSRCS:.c=.o)
OBJS+=$(CPPSRCS:%.cpp=%.o)

all: $(EXEC)

$(EXEC):  $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(LIBS) $(LDFLAGS)

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@
%.o: %.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f *.o *.a $(EXEC) $(OBJS) *.tar *.tar.gz libklt.a feat*.ppm feat*.txt
	rm -f feat*.ft feat*.fl *~ gmon.out pin.log

