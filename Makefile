PROJECT    = flappy

OPTIMIZE   = -Os
DEFS       =
LIBS       = -lncurses

CC        ?= gcc

CFLAGS_    = -g -Wall $(OPTIMIZE) $(DEFS) $(CFLAGS)
LDFLAGS    =

all: $(PROJECT)

$(PROJECT): main.o
	$(CC) $(CFLAGS_) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS_) -o $@ -c $^

clean:
	rm -f $(PROJECT) *.o *~
