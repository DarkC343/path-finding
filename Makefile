main: main.c
	gcc `pkg-config --cflags gtk+-3.0` main.c gui.c pathfinder.c -o main `pkg-config --libs gtk+-3.0` -Iinclude/
	gcc `pkg-config --cflags gtk+-3.0` designer.c -o designer `pkg-config --libs gtk+-3.0` -Iinclude/

