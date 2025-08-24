INC = ../computer/
OBJ = part1.o ../computer/comp.o

part1: $(OBJ)
	gcc $^ -o $@ -g

%.o : %.c
	gcc $< -c -o $@ -g -I$(INC)

test1: part1
	./part1 example
