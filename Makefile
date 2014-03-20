PROJECT    = flappy

OPTIMIZE   = -Os
DEFS       =
LIBS       = -lncurses

CC         = gcc

CFLAGS     = -g -Wall $(OPTIMIZE) $(DEFS)
LDFLAGS    =

all: $(PROJECT)

$(PROJECT): main.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

#main.o: main.c
#	$(CC) $(CFLAGS) -o $@ -c $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean:
	rm -f $(PROJECT) *.o *~
