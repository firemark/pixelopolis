VERSION?=DEBUG
CC = gcc
YACC = yacc
LEX = lex
OBJS  = main img draw draw_poly draw_builder css.y css.l hash
OBJS += css_func css_debug css_eval css_free css_make_new_rule
OBJS += angle_iter
DRAW_BUILDER_OBJS  = basic utils draw_obj
DRAW_BUILDER_OBJS += filler series series_utils void
DRAW_BUILDER_OBJS += cube cone cylinder pyramid triangle
DRAW_BUILDER_OBJS += wall floor texture
CSS_DRAW_OBJS  = component base texture
CSS_DRAW_OBJS += cube cone cylinder pyramid triangle
CSS_DRAW_OBJS += series void
TESTS = hash css_eval
OBJS_PATH = $(foreach obj,$(OBJS),out/$(obj).o)
OBJS_PATH += $(foreach obj,$(DRAW_BUILDER_OBJS),out/draw_builder/$(obj).o)
OBJS_PATH += $(foreach obj,$(CSS_DRAW_OBJS),out/css_draw/$(obj).o)
TESTS_PATH = $(foreach test,$(TESTS),tests_out/test_$(test))
LIBS = -lm -lpng
TEST_LIBS = -lm -lpng -lcheck
INCLUDES = -Iinclude

ifeq ($(VERSION),PROD)
FLAGS = -O3 -g -Wall
else
FLAGS = -g -Wall #-fsanitize=address,undefined
endif

all: make_dirs yacc pixelopolis

make_dirs:
	mkdir out -p
	mkdir out/draw_builder/ -p
	mkdir out/css_draw/ -p
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

tests_out/test_css_eval: tests/test_css_eval.c out/css_eval.o out/css_func.o out/css_free.o out/css.l.o out/css.y.o
	$(CC) tests/test_css_eval.c -o $(FLAGS) -c -o $@.o $(TEST_LIBS) $(INCLUDES)
	$(CC) $@.o out/css_eval.o out/css_func.o out/css_free.o out/css.l.o out/css.y.o -o $@ $(TEST_LIBS)

out/draw_builder/%.o: src/draw_builder/%.c
	$(CC) $< $(FLAGS) -c -o $@ $(INCLUDES)

out/css_draw/%.o: src/css_draw/%.c
	$(CC) $< $(FLAGS) -c -o $@ $(INCLUDES)

out/%.o: src/%.c
	$(CC) $< $(FLAGS) -c -o $@ $(INCLUDES)

clear:
	rm -f out/*.o pixelopolis
	rm -f out/draw_builder/*.o pixelopolis
	rm -f out/css_draw/*.o pixelopolis
	rm -f tests_out/*
