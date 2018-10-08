CC = gcc
OBJS = main.o img.o
LIBS = -lm -lpng
INCLUDES = -Iinclude

all: pixelopolis 

pixelopolis: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS) 

%.o: src/%.c
	$(CC) $< -c -o $@ $(INCLUDES)

clean:
	rm -f *.o pixelopolis
