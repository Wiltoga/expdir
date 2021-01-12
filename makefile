# PathToMake -f PathToMakeFile GCC=PathToGCC workspace=PathToProjectFolder
# -f, GCC and workspace are optionnal

CC := gcc
workspace := 
FLAGS := 



all: bin $(workspace)obj/main.o $(workspace)obj/fileUtils.o $(workspace)obj/conManagement/color.o $(workspace)obj/conManagement/consoleManagement.o $(workspace)obj/conManagement/stringAnsiManagement.o
	$(CC) $(FLAGS) -o bin/expdir $(workspace)obj/main.o $(workspace)obj/fileUtils.o $(workspace)obj/conManagement/color.o $(workspace)obj/conManagement/consoleManagement.o $(workspace)obj/conManagement/stringAnsiManagement.o


$(workspace)obj/main.o : obj $(workspace)src/main.c
	$(CC) $(FLAGS) -o $(workspace)obj/main.o -c $(workspace)src/main.c

$(workspace)obj/fileUtils.o : obj $(workspace)src/fileUtils.c
	$(CC) $(FLAGS) -o $(workspace)obj/fileUtils.o -c $(workspace)src/fileUtils.c

$(workspace)obj/conManagement/color.o : conManagement $(workspace)src/conManagement/color.c
	$(CC) $(FLAGS) -o $(workspace)obj/conManagement/color.o -c $(workspace)src/conManagement/color.c

$(workspace)obj/conManagement/consoleManagement.o : conManagement $(workspace)src/conManagement/consoleManagement.c
	$(CC) $(FLAGS) -o $(workspace)obj/conManagement/consoleManagement.o -c $(workspace)src/conManagement/consoleManagement.c

$(workspace)obj/conManagement/stringAnsiManagement.o : conManagement $(workspace)src/conManagement/stringAnsiManagement.c
	$(CC) $(FLAGS) -o $(workspace)obj/conManagement/stringAnsiManagement.o -c $(workspace)src/conManagement/stringAnsiManagement.c

bin:
ifeq ("$(wildcard $(workspace)bin)", "")
	mkdir $(workspace)bin
endif

obj:
ifeq ("$(wildcard $(workspace)obj)", "")
	mkdir $(workspace)obj
endif

conManagement: obj
ifeq ("$(wildcard $(workspace)obj/conManagement)", "")
	mkdir $(workspace)obj/conManagement
endif

clean:
	rm -rf obj
	rm -rf bin
