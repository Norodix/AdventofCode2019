OBJ = comp.o
INC = '.'

ifdef DBG
	DEF += -DDEBUG_INST
endif

all: runner

%: $(OBJ) %.o
	gcc -I$(INC) $^ -o $@ -g

%.o : %.c
	gcc -I$(INC) $< -c -o $@ -g $(DEF)
