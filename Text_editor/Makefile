# Makefile for Commander project

text_editor: main.o read_file.o write_file.o sig_winch.o
	gcc -o text_editor main.o read_file.o write_file.o sig_winch.o -lncurses

main.o: main.c
	gcc -c main.c -lncurses

read_file.o: read_file.c
	gcc -c read_file.c -lncurses

write_file.o: write_file.c
	gcc -c write_file.c -lncurses

sig_winch.o: sig_winch.c
	gcc -c sig_winch.c -lncurses

clean:
	rm -f *.o text_editor
