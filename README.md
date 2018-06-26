# Shim
## Description
The Shim program interfaces with any unix shell to offer coloration, hints and auto-completion for your line input. It is not a new implementation of a shell and you can use any shell you want with it.

## Prerequisites
* A Unix-based system : Shim uses some "unistd.h" functions, so unfortunately it won't work on Windows. Nonetheless, Shim was developed and tested under a Ubuntu system and there is no known reason it should fail under any other Unix-based distribution.
* A compiler with C++17 implementation and the file system in the standard enabled (latest versions of g++ and clang++ have it all).
* The replxx library is required : https://github.com/AmokHuginnsson/replxx (you would simply need to download its sources, use cmake to generate the makefile, then `sudo make install`).

## Installation
Clone or download the repository and then execute the script "install.sh".

## How to use it?
There is some options you can use:
- -s or --shell followed by the name of the shell and arguments (default is "bash -sl")
- -N or --no-history disables history searches for hints and completion
- --paths=none disables Shim to use the files in '/bin' and '/usr/bin' "PATH" in the hints and the completion
-  --paths=all enables Shim to use all files in the paths contained in the PATH environment variable for hints and completion

Example:
shim -s bash -Np

When Shim is started, the shortcuts are similar to other shells and the commands are the same as in the used shell.

# How does it work?
First it initialize the replxx library and the informations used in the hints, auto-completion and coloration.
Then it creates a pipe (2 linked file descriptors), the father process will be the writer.
After it creates a child process wich:
- close the standard input so the shell i will start later won't read it and does not create conflict with the replexx library which will read the standard input the the father process.
- redirect the read file descriptor of the pipe on the standard input, so when the father will write in the pipe it will be read by the shell.
- start a shell
Then the father will read the standard input, put some color etc and when the user confirm a line it will write it in the pipe.
In order to synchronize the 2 process, the father process will also write a command saying to send a signal (SIGUSR1 using kill) to himself and then wait until it receives this signal to be sure to wait the end of the command.

# Future
- Hints/auto-completion more accurate.
- Coloration of the binaries to differenciate them better.
- Coloration of the options beginning by - or --

# Bugs
If you find bugs tell me and I'll try to fix them when I have time.

# Thanks
This library: https://github.com/AmokHuginnsson/replxx is pretty amazing and i would not have done this without it.
Noé for the name of the project.
Erzender for the multiple corrections in this readme.

