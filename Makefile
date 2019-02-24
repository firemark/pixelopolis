CC = gcc
YACC = yacc
LEX = lex
OBJS = main.o img.o draw.o draw_shape.o css.y.o css.l.o css_func.o
LIBS = -lm -lpng
FLAGS = -g
INCLUDES = -Iinclude

all: yacc pixelopolis

yacc:
	$(YACC) src/css.y --defines=include/css.y.h --output=src/css.y.c
	$(LEX) -t src/css.l > src/css.l.c

pixelopolis: $(OBJS)
	$(CC) $(OBJS) $(FLAGS) -o $@ $(LIBS)

%.o: src/%.c
	$(CC) $< $(FLAGS) -c -o $@ $(INCLUDES)

clean:
	rm -f *.o pixelopolis
