VERSION?=DEBUG
CC = gcc
YACC = yacc
LEX = lex
OBJS = main img draw draw_shape css.y css.l css_func css_debug css_draw hash
TESTS = hash
OBJS_PATH = $(foreach obj,$(OBJS),out/$(obj).o)
TESTS_PATH = $(foreach test,$(TESTS),tests_out/test_$(test))
LIBS = -lm -lpng
TEST_LIBS = -lm -lpng -lcheck
INCLUDES = -Iinclude

ifeq ($(VERSION),PROD)
FLAGS = -O3 -g
else
FLAGS = -g -fsanitize=address,undefined -fno-omit-frame-pointer
endif

all: make_dirs yacc pixelopolis

make_dirs:
	mkdir out -p
	mkdir tests_out -p

yacc: src/css.y src/css.l
	$(YACC) src/css.y --defines=include/css.y.h --output=src/css.y.c
	$(LEX) -t src/css.l > src/css.l.c

pixelopolis: $(OBJS_PATH)
	$(CC) $(OBJS_PATH) $(FLAGS) -o $@ $(LIBS)

tests: $(TESTS_PATH)

tests_out/test_hash: tests/test_hash.c out/hash.o
	$(CC) tests/test_hash.c -o $(FLAGS) -c -o $@.o $(TEST_LIBS) $(INCLUDES)
	$(CC) $@.o out/hash.o -o $@ $(TEST_LIBS)

out/%.o: src/%.c
	$(CC) $< $(FLAGS) -c -o $@ $(INCLUDES)

clear:
	rm -f out/*.o pixelopolis
	rm -f tests_out/*
