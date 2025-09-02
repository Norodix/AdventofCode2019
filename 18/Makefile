INC = -I. -I../hashmap.c/
OBJ = ../hashmap.c/hashmap.o
DEF =
DEPS=$(wildcard $(INC)/*.h)
DEPS+=$(wildcard *.h)
OPS=-O3 -lm

ifdef DBG
	DEF += -DDEBUG_INST
endif

all: part1 part2

part2: $(OBJ) part2.o

part1: $(OBJ) part1.o

%: $(OBJ) %.o
	gcc $^ -o $@ -g $(OPS)

%.o : %.c $(DEPS)
	gcc $< -c -o $@ -g $(INC) $(DEF) $(OPS)

test1: part1
	./part1 example2

test2: part2
	./part2 example_p2_1
	@echo expected 8
	./part2 example_p2_2
	@echo expected 24
	./part2 example_p2_3
	@echo expected 32
	@echo This example file does not meet the quadrant expectations so its okay if it fails
	./part2 example_p2_4
	@echo expected 72

