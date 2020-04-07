main: main.c
	x86_64-w64-mingw32-gcc `x86_64-w64-mingw32-pkg-config --cflags gtk+-3.0` main.c gui.c pathfinder.c -o main `x86_64-w64-mingw32-pkg-config --libs gtk+-3.0` -Iinclude/
	x86_64-w64-mingw32-gcc `x86_64-w64-mingw32-pkg-config --cflags gtk+-3.0` designer.c -o designer `x86_64-w64-mingw32-pkg-config --libs gtk+-3.0` -Iinclude/


#main: main.c
#	gcc `pkg-config --cflags gtk+-3.0` main.c gui.c pathfinder.c -o main `pkg-config --libs gtk+-3.0` -Iinclude/
#	gcc `pkg-config --cflags gtk+-3.0` designer.c -o designer `pkg-config --libs gtk+-3.0` -Iinclude/

