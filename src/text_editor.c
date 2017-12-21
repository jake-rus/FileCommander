/*
 * File_Editor
 *
 *  Created on: 17.12.2017
 *      Author: jake
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
#include <unistd.h>

void sig_winch(int signo) // обработчик сигнала SIGWINCH
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char*) &size);
	resizeterm(size.ws_row, size.ws_col);
}

void readFile(int fd,WINDOW *wnd) 		// буферизированное чтение из файла
{
    unsigned char buffer[1];
    int bytes_read;

    if ((bytes_read = read(fd, buffer, 1)) !=0)
        {
    	wprintw(wnd,"%s", buffer);
        readFile(fd,wnd);
        }
}

void writeFile(int fd,WINDOW *wnd) 		// буферизированная запись в файл
{
	int x,y;
	int buffer[1];

	for (y=0;y<=wnd->_maxy; y++){
	for (x=0;x<=wnd->_maxx; x++){
    *buffer = mvwinch(wnd,y,x);
	write(fd, buffer,1);};
	buffer[0]=0;
	write(fd,buffer,1);}
}

int main(int argc, char **argv) 	// ТЕКСТОВЫЙ РЕДАКТОР
{

	WINDOW *wnd;
	WINDOW * wnd_editorMenu; // Меню редактора
	WINDOW * wnd_editor;	// окно редактора
	short highEditorMenu=2; // высота меню редактора
	int c; // для считывания символов с клавиатуры
	int x=0,y=0;// координаты
	int filedesc = 0;// файловый дескриптор

	initscr(); // инициализация структуры данных в режиме ncurses
	signal (SIGWINCH, sig_winch);
	cbreak(); // включение неканонического режима ввода данных (без ENTER)

	start_color();
	refresh();

	wnd = newwin(0,0,0,0); // фоновое окно размером с экран терминала
	//box(wnd,'||','=');
	wclear(wnd);
	wrefresh(wnd);

	wnd_editorMenu = derwin(wnd,highEditorMenu,wnd->_maxx/2-2,1,1);// создание окна меню
	wclear(wnd_editorMenu); // очистка окна
	init_pair(2,COLOR_RED, COLOR_CYAN);  // цветовая пара для меню
	wattron(wnd_editorMenu,COLOR_PAIR(2) | A_BOLD);
	wprintw(wnd_editorMenu,"F2 - Save file		ESC - Close file/Exit");
	wrefresh(wnd_editorMenu);

	wnd_editor = derwin (wnd,wnd->_maxy-highEditorMenu-2,
			wnd->_maxx/2-2,wnd_editorMenu->_maxy+2,1); // создание окна редактора
	wclear(wnd_editor);
	wrefresh(wnd_editor);

	if (((filedesc = open(argv[1],O_RDONLY)) != -1) && (filedesc != 0))	// открытие файла
	readFile(filedesc,wnd_editor); // чтение файла
		else perror("file open"); // случай ошибки открытия файла
	close(filedesc);

	curs_set(1); // видимый курсор
	keypad(wnd_editor, TRUE); // включение режима работы с функциональными клавишами
	wmove(wnd_editor,y=0,x=0);
	wrefresh (wnd_editor);
	noecho();

	while ((c=wgetch(wnd_editor)) != 27) switch(c)
	 {
	 	 case KEY_RIGHT:				// перемещение по тексту
	 		 x++;
	 		 if (x > wnd_editor->_maxx) {y++;x=0;}
	 		 wmove(wnd_editor,y,x);
	 		 wrefresh(wnd_editor);break;

	 	 case KEY_LEFT:
	 		 x--;
	 		 if (x < 0) {y--;x=wnd_editor->_maxx;}
	 		 wmove(wnd_editor,y,x);
	 		 wrefresh(wnd_editor);break;

	 	 case KEY_UP:
	 		 y--;
	 		 wmove(wnd_editor,y,x);
	 		 wrefresh(wnd_editor);break;

		 case KEY_DOWN:
			 y++;
			 wmove(wnd_editor,y,x);
			 wrefresh(wnd_editor);break;

		 case KEY_F(2):
			if (((filedesc = open(argv[1],O_WRONLY |O_TRUNC)) != -1) && (filedesc != 0))// открытие для записи
			{writeFile(filedesc,wnd_editor); // сохранение файла
		 	close(filedesc);} // закрытие файла
			else perror("file open for write");
		 	wmove(wnd_editor,y,x);break;


		 default:
			 wprintw(wnd_editor,"%c",c); // ввод символов в текст
			 x++;
			 if (x > wnd_editor->_maxx) {y++;x=0;}
			 wrefresh(wnd_editor);break;
	 };

	filedesc=0;  // очистка переменной дескриптора

	wclear(wnd_editorMenu); // очистка окон
	wclear(wnd_editor);
	delwin(wnd_editorMenu); // удаление окон
	delwin(wnd_editor);
	wclear(wnd);
	delwin(wnd);
	//endwin(); // выход из режима curses
	exit(EXIT_SUCCESS);
}

