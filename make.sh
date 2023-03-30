# /usr/bin/bash

g++ -O3 -c TUI.cpp render.cpp readGout.cpp -lcurses -std=c++11 -D GOUT
g++ -O3 -o TUI TUI.o render.o readGout.o  -lcurses -std=c++11 -D GOUT
