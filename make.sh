# /usr/bin/bash

clang++ -Wall -O2 -static -c TUI.cpp render.cpp readGout.cpp -l:libncurses.a -ltinfo -std=c++11 -D GOUT 
clang++ -static -o molview TUI.o render.o readGout.o  -l:libncurses.a -ltinfo  -std=c++11 -D GOUT 
