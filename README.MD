# expdir

`expdir` is a tool to navigate easily between folders using a terminal.

here is an example of what it looks like :

![](https://i.gyazo.com/11fee308061aac5e65b89dcf5b0c45f4.png)

You can control the cursor using the arrow keys and validate the directory using the space key.

## Usage

Simply call `expdir` to start the explorer. You can still call `expdir --help` for a list of all the options available.

Commands :
- `Space` : Validate the directory go change the current working directory.
- `Ctrl+X` : Stop the application, do not move to directory.
- `Backspace` : Refresh the directory.
- `Tab` : Go back to the parent of the current folder.

#### Custom aliases

You can also add aliases to the directories : you will have to edit `/etc/expdir/aliases` for system-wide aliases, or `~/.config/expdir/aliases` for user-wide aliases. Note : you will have to create the `~/.config/expdir/aliases` file.
A very simple alias (that is already added system-wide upon installation), for example :
```sh
%HOME%=~
```

This will simply change the home folder of an user to `~`.

## Installation

Go to the [latest release](https://github.com/WildGoat07/expdir/releases/latest) and follow the steps.
After the intallation, you have to reboot for it to take effect.

## Build

First install the dependencies to build the project :
```sh
sudo apt update
sudo apt install build-essential
```
To build, clone this repository, then build using the makefile.
You can also generate the .deb using `make deb`. It will be in the ./package folder.
```
git clone https://github.com/WildGoat07/expdir
cd expdir
make deb
```
