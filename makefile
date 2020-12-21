# PathToMake -f PathToMakeFile GCC=PathToGCC workspace=PathToProjectFolder
# -f, GCC and workspace are optionnal

CC := gcc
workspace := 
FLAGS := -g -Wall


all: bin $(workspace)obj/main.o $(workspace)obj/fileUtils.o
	$(CC) $(FLAGS) -o bin/expdir $(workspace)obj/main.o $(workspace)obj/fileUtils.o


$(workspace)obj/main.o : obj $(workspace)src/main.c
	$(CC) $(FLAGS) -o $(workspace)obj/main.o -c $(workspace)src/main.c

$(workspace)obj/fileUtils.o : obj $(workspace)src/fileUtils.c
	$(CC) $(FLAGS) -o $(workspace)obj/fileUtils.o -c $(workspace)src/fileUtils.c

bin:
ifeq ("$(wildcard $(workspace)bin)", "")
	mkdir $(workspace)bin
endif

obj:
ifeq ("$(wildcard $(workspace)obj)", "")
	mkdir $(workspace)obj
endif

rebuild: clean all

clean:
	rm -rf obj
	rm -rf bin