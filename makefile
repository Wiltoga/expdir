CC := gcc
workspace := .
FLAGS := -g -Wall

OBJ := $(workspace)/obj
BIN := $(workspace)/bin
SRC := $(workspace)/src

SRC_EXPDIR := $(SRC)/expdir
SRC_CONSOLE_M := $(SRC)/expdir/conManagement
SRC_SETUP := $(SRC)/setup

OBJ_EXPDIR := $(OBJ)/expdir
OBJ_CONSOLE_M := $(OBJ)/expdir/consoleManagement
OBJ_SETUP := $(OBJ)/setup

BIN_EXPDIR := $(BIN)/expdir
BIN_SETUP := $(BIN)/setup

EXPDIR_OUTPUT := $(BIN_EXPDIR)/expdirapp
SETUP_OUTPUT := $(BIN_SETUP)/install


all: expdir

expdir: bin $(OBJ_EXPDIR)/main.o $(OBJ_EXPDIR)/fileUtils.o conManagement
	$(CC) $(FLAGS) -o $(EXPDIR_OUTPUT) $(OBJ_EXPDIR)/main.o $(OBJ_EXPDIR)/fileUtils.o $(OBJ_CONSOLE_M)/color.o $(OBJ_CONSOLE_M)/consoleManagement.o $(OBJ_CONSOLE_M)/stringAnsiManagement.o

$(OBJ_EXPDIR)/main.o : obj $(SRC_EXPDIR)/main.c
	$(CC) $(FLAGS) -o $(OBJ_EXPDIR)/main.o -c $(SRC_EXPDIR)/main.c

$(OBJ_EXPDIR)/fileUtils.o : obj $(SRC_EXPDIR)/fileUtils.c
	$(CC) $(FLAGS) -o $(OBJ_EXPDIR)/fileUtils.o -c $(SRC_EXPDIR)/fileUtils.c

conManagement: $(OBJ_CONSOLE_M)/color.o $(OBJ_CONSOLE_M)/consoleManagement.o $(OBJ_CONSOLE_M)/stringAnsiManagement.o

$(OBJ_CONSOLE_M)/color.o : obj $(SRC_CONSOLE_M)/color.c
	$(CC) $(FLAGS) -o $(OBJ_CONSOLE_M)/color.o -c $(SRC_CONSOLE_M)/color.c

$(OBJ_CONSOLE_M)/consoleManagement.o : obj $(SRC_CONSOLE_M)/consoleManagement.c
	$(CC) $(FLAGS) -o $(OBJ_CONSOLE_M)/consoleManagement.o -c $(SRC_CONSOLE_M)/consoleManagement.c

$(OBJ_CONSOLE_M)/stringAnsiManagement.o : obj $(SRC_CONSOLE_M)/stringAnsiManagement.c
	$(CC) $(FLAGS) -o $(OBJ_CONSOLE_M)/stringAnsiManagement.o -c $(SRC_CONSOLE_M)/stringAnsiManagement.c

setup: 

bin:
ifeq ("$(wildcard $(BIN))", "")
	mkdir $(BIN)
endif
ifeq ("$(wildcard $(BIN_EXPDIR))", "")
	mkdir $(BIN_EXPDIR)
endif
ifeq ("$(wildcard $(BIN_SETUP))", "")
	mkdir $(BIN_SETUP)
endif

obj:
ifeq ("$(wildcard $(OBJ))", "")
	mkdir $(OBJ)
endif
ifeq ("$(wildcard $(OBJ_EXPDIR))", "")
	mkdir $(OBJ_EXPDIR)
endif
ifeq ("$(wildcard $(OBJ_CONSOLE_M))", "")
	mkdir $(OBJ_CONSOLE_M)
endif
ifeq ("$(wildcard $(OBJ_SETUP))", "")
	mkdir $(OBJ_SETUP)
endif

clean:
	rm -rf obj
	rm -rf bin

rebuild: clean obj bin all