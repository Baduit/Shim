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
- --paths=all enables Shim to use all files in the paths contained in the PATH environment variable for hints and completion
- -h or --hints followed by the number of line used by the hints

Example:
shim -s bash -Np

When Shim is started, the shortcuts are similar to other shells and the commands are the same as in the used shell.

# How does it work?
When executed, Shim initializes the replxx library and defines the information used for hints, auto-completion and coloration. It then creates a pipeline (2 linked file descriptors), the main process will be the writer. After it creates a child process which:
- closes the standard input so that conflicts are avoided between the replxx library and the used shell reading on that same input.
- redirects the read file descriptor of the pipe on the standard input, so that when the main process will write in the pipe it will be read by the shell.
- starts a shell

The main process will read from the standard input, make some line style edition and wait for the user to confirm a line to write it in the pipe. In order to synchronize both process, the main process will also write a command to trigger himself with a signal (SIGUSR1 using the kill command) and then wait until it receives this signal to be sure to wait until the end of the command.

# Future
- Hints/auto-completion more accurate.

# Bugs
If you find bugs tell me and I'll try to fix them when I have time.

# Thanks
- This library: https://github.com/AmokHuginnsson/replxx is pretty amazing and i would not have done this without it.
- Noé for the name of the project.
- Erzender for the multiple corrections in this readme.

