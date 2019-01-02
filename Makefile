CC = gcc
OBJS = main.o img.o
LIBS = -lm -lpng
FLAGS = -g
INCLUDES = -Iinclude

all: pixelopolis 

pixelopolis: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $@ $(LIBS)

%.o: src/%.c
	$(CC) $< $(FLAGS) -c -o $@ $(INCLUDES)

clean:
	rm -f *.o pixelopolis
