CC = gcc
YACC = yacc
LEX = lex
OBJS = main img draw draw_shape css.y css.l css_func css_debug
OBJS_PATH = $(foreach obj,$(OBJS),out/$(obj).o)
LIBS = -lm -lpng
FLAGS = -g
INCLUDES = -Iinclude

all: make_dirs yacc pixelopolis

make_dirs: out/
	mkdir out -p

yacc: src/css.y src/css.l
	$(YACC) src/css.y --defines=include/css.y.h --output=src/css.y.c
	$(LEX) -t src/css.l > src/css.l.c

pixelopolis: $(OBJS_PATH)
	$(CC) $(OBJS_PATH) $(FLAGS) -o $@ $(LIBS)

out/%.o: src/%.c
	$(CC) $< $(FLAGS) -c -o $@ $(INCLUDES)

clear:
	rm -f *.o pixelopolis
