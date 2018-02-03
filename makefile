#rshell makefile

#Variables
COMPILE = g++
FLAGS = -Wall -Werror -ansi -pedantic
OBJS = rshell.o

#Targets

all: rshell	

rshell: 
	@mkdir -p bin
	$(COMPILE) $(FLAGS) src/rshell.cpp src/Base.h src/Base.cpp src/Operation.h src/Operation.cpp -o bin/rshell

clean:
	-rm -rf bin/
