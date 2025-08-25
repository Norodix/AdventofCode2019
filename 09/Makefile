INC = ../computer/
OBJ = ../computer/comp.o
DEF = -DCUSTOM_INPUT -DCUSTOM_OUTPUT
DEPS=$(wildcard $(INC)/*.h)
DEPS+=$(wildcard *.h)

ifdef DBG
	DEF += -DDEBUG_INST
endif

all: part1 part2

part2: $(OBJ) part2.o

part1: $(OBJ) part1.o

%: $(OBJ) %.o
	gcc $^ -o $@ -g

%.o : %.c $(DEPS)
	gcc $< -c -o $@ -g -I$(INC) $(DEF)

test1: part1
	./part1 example
	./part1 example2
	./part1 example3
