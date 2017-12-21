# Makefile for Commander project

commander: main.o dir_reader.o navigation.o select_line.o sig_winch.o
	gcc -o commander main.o dir_reader.o navigation.o select_line.o sig_winch.o -lncurses

main.o: main.c
	gcc -c main.c -lncurses

dir_reader.o: dir_reader.c
	gcc -c dir_reader.c -lncurses

navigation.o: navigation.c
	gcc -c navigation.c -lncurses

select_line.o: select_line.c
	gcc -c select_line.c -lncurses

sig_winch.o: sig_winch.c
	gcc -c sig_winch.c -lncurses

clean:
	rm -f *.o commander
