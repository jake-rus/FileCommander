# Makefile for Commander project

commander: main.o dir_reader.o navigation.o select_line.o sig_winch.o copy_file.o status_copy.o
	gcc -o commander main.o dir_reader.o navigation.o select_line.o sig_winch.o copy_file.o status_copy.o -lncurses -lpthread

main.o: main.c
	gcc -c main.c -lncurses -lpthread

dir_reader.o: dir_reader.c
	gcc -c dir_reader.c -lncurses

navigation.o: navigation.c
	gcc -c navigation.c -lncurses

select_line.o: select_line.c
	gcc -c select_line.c -lncurses

sig_winch.o: sig_winch.c
	gcc -c sig_winch.c -lncurses

copy_file.o: copy_file.c
	gcc -c copy_file.c -lncurses -lpthread

status_copy.o: status_copy.c
	gcc -c status_copy.c -lncurses -lpthread

clean:
	rm -f *.o commander
