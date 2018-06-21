# Shim
## Description
Shim read line which is interfaced with a shell to offer coloration, hints and auto completion. It is not a new implementation of a shell and you can use any shell you want with it.

## Installation
Clone or download the repository and then execute the script "install.sh".

## How to use it?
There is some options you can use:
- -s or --shell followed by the name of the shell and its argument (default is "bash -sl")
- -N or --no-history if you want to desactivate the use of the history in the hints and the completion
- -p or --paths if you want to activate the use of all files in the paths set in the variable environnement "PATH" in the hints and the completion

Example:
shim -s bash -Np

When shim is started, the shortcuts are similar to others shells and the command are the same as the shell used.

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
If you find bugs tell me and I'll try to fix them if I have the time.

# Thanks
This library: https://github.com/AmokHuginnsson/replxx is pretty amazing and i would not have done this without it.
Noé for the name of the project.

