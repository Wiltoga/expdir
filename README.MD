# expdir

`expdir` is a tool to navigate easily between folders using a terminal.

here is an example of what it looks like :

![](https://i.gyazo.com/11fee308061aac5e65b89dcf5b0c45f4.png)

You can control the cursor using the arrow keys and validate the directory using the space key.

## Installation

To install, clone this repository, then build using the makefile.
Once this is done, you have to copy the content of [bashrc](bashrc) to your `~/.bashrc` file, at the end. If the makefile does not work, try a `make clean` before. Edit the `dir` variable for the location of the `bin` folder (where is the `expdir` executable). Now, simply restart the terminal. Type `expdir --help` for more info.
