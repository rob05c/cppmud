CC=g++
CPPFLAGS=-g -pthread -I/sw/include/root -fpermissive
LDFLAGS=-pthread
#LDLIBS=

cppmud: main.o repl.o util.o world.o cmd.o player.o dir.o room.o object.o
	$(CC) $(LDFLAGS) -o cppmud main.o repl.o util.o world.o cmd.o player.o dir.o room.o object.o

main.o: main.cpp main.hpp
	$(CC) $(CPPFLAGS) -c main.cpp

repl.o: repl.cpp repl.hpp
	$(CC) $(CPPFLAGS) -c repl.cpp

util.o: util.cpp util.hpp
	$(CC) $(CPPFLAGS) -c util.cpp

world.o: world.cpp world.hpp
	$(CC) $(CPPFLAGS) -c world.cpp

cmd.o: cmd.cpp cmd.hpp
	$(CC) $(CPPFLAGS) -c cmd.cpp

player.o: player.cpp player.hpp
	$(CC) $(CPPFLAGS) -c player.cpp

dir.o: dir.cpp dir.hpp
	$(CC) $(CPPFLAGS) -c dir.cpp

room.o: room.cpp room.hpp
	$(CC) $(CPPFLAGS) -c room.cpp

object.o: object.cpp object.hpp
	$(CC) $(CPPFLAGS) -c object.cpp

clean:
	rm -rf *.o dataman
