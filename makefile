workspace := .
FLAGS := -Wall
CC := gcc $(FLAGS)

OBJ := $(workspace)/obj
BIN := $(workspace)/bin
SRC := $(workspace)/src

PACKAGE := $(workspace)/expdir
VERSION := 1.0.3

SRC_EXPDIR := $(SRC)
SRC_CONSOLE_M := $(SRC)/conManagement

OBJ_EXPDIR := $(OBJ)
OBJ_CONSOLE_M := $(OBJ)/consoleManagement

BIN_EXPDIR := $(BIN)

EXPDIR_OUTPUT := $(BIN_EXPDIR)/expdirapp

DEB_DIR := $(workspace)/package

DEB_OUTPUT := $(DEB_DIR)/expdir-$(VERSION).deb


all: expdir

expdir: bin $(OBJ_EXPDIR)/main.o $(OBJ_EXPDIR)/fileUtils.o $(OBJ_EXPDIR)/aliases.o conManagement
	$(CC) -o $(EXPDIR_OUTPUT) $(OBJ_EXPDIR)/main.o $(OBJ_EXPDIR)/aliases.o $(OBJ_EXPDIR)/fileUtils.o $(OBJ_CONSOLE_M)/color.o $(OBJ_CONSOLE_M)/consoleManagement.o $(OBJ_CONSOLE_M)/stringAnsiManagement.o

$(OBJ_EXPDIR)/main.o : obj $(SRC_EXPDIR)/main.c
	$(CC) -o $(OBJ_EXPDIR)/main.o -c $(SRC_EXPDIR)/main.c

$(OBJ_EXPDIR)/fileUtils.o : obj $(SRC_EXPDIR)/fileUtils.c
	$(CC) -o $(OBJ_EXPDIR)/fileUtils.o -c $(SRC_EXPDIR)/fileUtils.c

$(OBJ_EXPDIR)/aliases.o : obj $(SRC_EXPDIR)/aliases.c
	$(CC) -o $(OBJ_EXPDIR)/aliases.o -c $(SRC_EXPDIR)/aliases.c

conManagement: $(OBJ_CONSOLE_M)/color.o $(OBJ_CONSOLE_M)/consoleManagement.o $(OBJ_CONSOLE_M)/stringAnsiManagement.o

$(OBJ_CONSOLE_M)/color.o : obj $(SRC_CONSOLE_M)/color.c
	$(CC) -o $(OBJ_CONSOLE_M)/color.o -c $(SRC_CONSOLE_M)/color.c

$(OBJ_CONSOLE_M)/consoleManagement.o : obj $(SRC_CONSOLE_M)/consoleManagement.c
	$(CC) -o $(OBJ_CONSOLE_M)/consoleManagement.o -c $(SRC_CONSOLE_M)/consoleManagement.c

$(OBJ_CONSOLE_M)/stringAnsiManagement.o : obj $(SRC_CONSOLE_M)/stringAnsiManagement.c
	$(CC) -o $(OBJ_CONSOLE_M)/stringAnsiManagement.o -c $(SRC_CONSOLE_M)/stringAnsiManagement.c

deb: all
ifeq ("$(wildcard $(PACKAGE)/DEBIAN)", "")
	mkdir -p $(PACKAGE)/DEBIAN
endif
	cp $(workspace)/control $(PACKAGE)/DEBIAN
	echo "Version: $(VERSION)" >> $(PACKAGE)/DEBIAN/control
ifeq ("$(wildcard $(PACKAGE)/usr/local/bin)", "")
	mkdir -p $(PACKAGE)/usr/local/bin
endif
ifeq ("$(wildcard $(PACKAGE)/var/cache/expdir)", "")
	mkdir -p $(PACKAGE)/var/cache/expdir
endif
ifeq ("$(wildcard $(DEB_DIR))", "")
	mkdir -p $(DEB_DIR)
endif
	cp $(EXPDIR_OUTPUT) $(PACKAGE)/usr/local/bin
	dpkg-deb -b $(PACKAGE)
	mv expdir.deb $(DEB_OUTPUT)

bin:
ifeq ("$(wildcard $(BIN_EXPDIR))", "")
	mkdir -p $(BIN_EXPDIR)
endif

obj:
ifeq ("$(wildcard $(OBJ_EXPDIR))", "")
	mkdir -p $(OBJ_EXPDIR)
endif
ifeq ("$(wildcard $(OBJ_CONSOLE_M))", "")
	mkdir -p $(OBJ_CONSOLE_M)
endif

clean:
	rm -rf $(OBJ)
	rm -rf $(BIN)
	rm -rf $(PACKAGE)/usr
	rm -rf $(PACKAGE)/DEBIAN
