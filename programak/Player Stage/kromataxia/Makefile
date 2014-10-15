CC=gcc

CFLAGS=`pkg-config --cflags playerc` `pkg-config --cflags playercore` 
LIBS =`pkg-config --libs playerc` -lm

TARGETS = kromataxia

all: $(TARGETS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm *.o *~ $(TARGETS)