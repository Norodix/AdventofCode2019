INC = .
OBJ =
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
	gcc $< -c -o $@ -g -I$(INC) $(DEF) $(OPS)

test1: part1
	./part1 example2

test2: part2
	./part2 example3

doc: index.html
index.html: notes.md
	pandoc --standalone --self-contained -f markdown+tex_math_dollars $< -t html5 --mathml -o $@

