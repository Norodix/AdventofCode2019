INC = ../computer/
OBJ = part1.o ../computer/comp.o
DEF = -DCUSTOM_INPUT -DCUSTOM_OUTPUT

part1: $(OBJ)
	gcc $^ -o $@ -g

%.o : %.c
	gcc $< -c -o $@ -g -I$(INC) $(DEF)

test1: part1
	./part1 example
	./part1 example2
	./part1 example3
