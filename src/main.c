/*
 ============================================================================
 Name        : main.c
 Author      : Jake
 Description : File Commander
 ============================================================================
 */

#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <panel.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>

enum side{left, right};
typedef enum side _side;

int commander(WINDOW *fwnd, _side side);

void sig_winch(int signo) // обработчик сигнала SIGWINCH
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char*) &size);
	resizeterm(size.ws_row, size.ws_col);
}

int main(int argc, char **argv)
{
	WINDOW *wnd;

	initscr(); // инициализация структуры данных в режиме ncurses
	signal (SIGWINCH, sig_winch);
	cbreak(); // включение неканонического режима ввода данных (без ENTER)

	curs_set(0); // невидимый курсор
	start_color();
	refresh();

	wnd = newwin(0,0,0,0);
	box(wnd,'||','=');
	wrefresh(wnd);

	commander(wnd, right); // вызов правого файлового менеджера
	commander(wnd, left);	// вызов левого файлового менеджера

	delwin(wnd);
	endwin();
	exit(EXIT_SUCCESS);
}
