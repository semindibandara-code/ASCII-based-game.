CC = gcc
CFLAGS = -std=c89 -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion -Wformat=2 -Wcast-align -Wstrict-prototypes -g
LDFLAGS =
OBJ=main.o game.o map.o util.o color.o terminal.o newSleep.o ll.o



all: escape

escape: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

main.o: main.c game.h map.h util.h color.h terminal.h newSleep.h
game.o: game.c game.h map.h util.h color.h terminal.h newSleep.h ll.h
map.o: map.c map.h
util.o: util.c util.h
ll.o: ll.c ll.h

color.o: color.c color.h
terminal.o: terminal.c terminal.h
newSleep.o: newSleep.c newSleep.h

clean:
	$(RM) -f $(OBJ) escape

