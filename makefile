workspace := .
FLAGS := -g -Wall
CC := gcc $(FLAGS)

OBJ := $(workspace)/obj
BIN := $(workspace)/bin
SRC := $(workspace)/src

PACKAGE := $(workspace)/expdir
VERSION := 1.0

SRC_EXPDIR := $(SRC)
SRC_CONSOLE_M := $(SRC)/conManagement

OBJ_EXPDIR := $(OBJ)
OBJ_CONSOLE_M := $(OBJ)/consoleManagement

BIN_EXPDIR := $(BIN)

EXPDIR_OUTPUT := $(BIN_EXPDIR)/expdirapp


all: expdir

expdir: bin $(OBJ_EXPDIR)/main.o $(OBJ_EXPDIR)/fileUtils.o conManagement
	$(CC) -o $(EXPDIR_OUTPUT) $(OBJ_EXPDIR)/main.o $(OBJ_EXPDIR)/fileUtils.o $(OBJ_CONSOLE_M)/color.o $(OBJ_CONSOLE_M)/consoleManagement.o $(OBJ_CONSOLE_M)/stringAnsiManagement.o

$(OBJ_EXPDIR)/main.o : obj $(SRC_EXPDIR)/main.c
	$(CC) -o $(OBJ_EXPDIR)/main.o -c $(SRC_EXPDIR)/main.c

$(OBJ_EXPDIR)/fileUtils.o : obj $(SRC_EXPDIR)/fileUtils.c
	$(CC) -o $(OBJ_EXPDIR)/fileUtils.o -c $(SRC_EXPDIR)/fileUtils.c

conManagement: $(OBJ_CONSOLE_M)/color.o $(OBJ_CONSOLE_M)/consoleManagement.o $(OBJ_CONSOLE_M)/stringAnsiManagement.o

$(OBJ_CONSOLE_M)/color.o : obj $(SRC_CONSOLE_M)/color.c
	$(CC) -o $(OBJ_CONSOLE_M)/color.o -c $(SRC_CONSOLE_M)/color.c

$(OBJ_CONSOLE_M)/consoleManagement.o : obj $(SRC_CONSOLE_M)/consoleManagement.c
	$(CC) -o $(OBJ_CONSOLE_M)/consoleManagement.o -c $(SRC_CONSOLE_M)/consoleManagement.c

$(OBJ_CONSOLE_M)/stringAnsiManagement.o : obj $(SRC_CONSOLE_M)/stringAnsiManagement.c
	$(CC) -o $(OBJ_CONSOLE_M)/stringAnsiManagement.o -c $(SRC_CONSOLE_M)/stringAnsiManagement.c

deb: all
ifeq ("$(wildcard $(PACKAGE)/usr/local/bin)", "")
	mkdir -p $(PACKAGE)/usr/local/bin
endif
	cp $(EXPDIR_OUTPUT) $(PACKAGE)/usr/local/bin
	dpkg-deb -b $(PACKAGE)
ifeq ("$(wildcard $(workspace)/packages)", "")
	mkdir $(workspace)/packages
endif
	mv expdir.deb $(workspace)/packages/expdir-$(VERSION).deb
	dpkg-scanpackages $(workspace)/packages | gzip -c9  > $(workspace)/packages/Packages.gz

$(OBJ_SETUP)/main.o : obj $(SRC_SETUP)/main.c
	$(CC) -o $(OBJ_SETUP)/main.o -c $(SRC_SETUP)/main.c

$(OBJ_SETUP)/bashrc.o : obj $(SRC_SETUP)/bashrc.sh
	$(OBJCOPY) $(SRC_SETUP)/bashrc.sh $(OBJ_SETUP)/bashrc.o

bin:
ifeq ("$(wildcard $(BIN_EXPDIR))", "")
	mkdir $(BIN_EXPDIR)
endif

obj:
ifeq ("$(wildcard $(OBJ_EXPDIR))", "")
	mkdir $(OBJ_EXPDIR)
endif
ifeq ("$(wildcard $(OBJ_CONSOLE_M))", "")
	mkdir $(OBJ_CONSOLE_M)
endif

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)
	rm -rf $(PACKAGE)/usr

rebuild: clean obj bin all
