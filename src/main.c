/*
 * File Commander
 *
 *  Created on: 18.12.2017
 *      Author: jake
 */
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <panel.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

typedef struct {			// структура лев/прав окна менеджера
	char id;		// идентификатор правого окна менеджера
	char dir_name[255]; // имя каталога
	int x,y;			// координаты курсора
} comm;

void sig_winch(int signo) // обработчик сигнала SIGWINCH
{
	struct winsize size;
	ioctl(fileno(stdout), TIOCGWINSZ, (char*) &size);
	resizeterm(size.ws_row, size.ws_col);
}

int selectLine(WINDOW *wnd,int y) // 	Выделение строчки
{
	int x;
	char buffer[1];
	init_pair(5,COLOR_YELLOW, COLOR_BLACK);
	wattron(wnd,COLOR_PAIR(5) | A_BOLD);

	for (x=0;x<=wnd->_maxx;x++)
	{
		*buffer = mvwinch(wnd,y,x);
		wprintw(wnd,"%c",*buffer);
	}
	wattroff(wnd,COLOR_PAIR(5));
	return 0;
};

WINDOW *commander(WINDOW *fwnd,comm *side, DIR *dir,struct dirent *entry)// чтение каталога
{
	WINDOW *wnd_commander;

	switch (side->id){ 		// варианты правого\левого менеджеров файлов
	case 1: wnd_commander = derwin(fwnd,fwnd->_maxy-2,fwnd->_maxx/2-2,1,fwnd->_maxx/2+1);
	break;
	default: wnd_commander = derwin(fwnd,fwnd->_maxy-2,fwnd->_maxx/2-2,1,1);
	break;}

	wclear(wnd_commander);	// очистка окна менеджера
	wattron(wnd_commander,A_STANDOUT);
	mvwprintw(wnd_commander, 0,0,"TAB - switch R/L, SPACE - select, ENT - open\n");
	wattroff(wnd_commander,A_STANDOUT);
	wattron(wnd_commander,A_UNDERLINE);
	wprintw(wnd_commander, "Dir: %s\n", side->dir_name);
	wattroff(wnd_commander,A_UNDERLINE);
	wattron(wnd_commander,A_BOLD | A_BLINK);
	wprintw(wnd_commander, "\n<= BACK ===\n");
	wattroff(wnd_commander,A_BOLD | A_BLINK);

    dir = opendir(side->dir_name);	// проверка открытия каталога
    if (!dir) {delwin(wnd_commander);perror("diropen");}
    else {
    while ( (entry = readdir(dir)) != NULL) {
        wprintw(wnd_commander,"%s [%s]\n",
            entry->d_name, (entry->d_type==DT_DIR)?"DIR":"file");
    };closedir(dir);};

    curs_set(1);
    keypad(wnd_commander, TRUE);
    wrefresh(wnd_commander);
    noecho();
    cbreak();

	return wnd_commander;
}

int dir_navigation(WINDOW *wnd, comm *side)	// перемещение по каталогу
{
	int c;

	while ((c=wgetch(wnd)) != 27)
	switch(c){
		case KEY_UP:
			(side->y)--;
			wmove(wnd,side->y,side->x);
			wrefresh(wnd); break;

		case KEY_DOWN:
			(side->y)++;
			wmove(wnd,side->y,side->x);
			wrefresh(wnd); break;

	    case ' ':				 // выделение строчки SPACE
	    	selectLine(wnd,side->y);
	    	wrefresh(wnd); break;

		case 9:				// переключение между окнами TAB
			return 0;break;
		case 10:			// вызов редактора ENT
			return 1;break;// либо переход на другой каталог
   		default:break;}
	return -1;
}

int main(int argc, char **argv)
{
	WINDOW *wnd, *wnd_commL, *wnd_commR;
	comm left = {0,{'/'},0,0};	//параметры левого окна
	comm right = {1,"/",0,0};	//параметры правого окна
	comm *pleft=&left, *pright=&right;	// указатели на параметры
	pid_t proc;           // переменная для pid текстового редактора
	struct dirent *entry; // записи в структуре каталога
	DIR *dir=NULL;				// указатель на каталог
	int i=0;
	char buff[255]={0};
	char n;

	initscr(); // инициализация структуры данных в режиме ncurses
	signal (SIGWINCH, sig_winch);
	cbreak(); // включение неканонического режима ввода данных (без ENTER)

	start_color();
	refresh();

	wnd = newwin(0,0,0,0); // фоновое окно размером с экран терминала
	box(wnd,'||','=');
	wrefresh(wnd);
	wnd_commR = commander(wnd,pright,dir,entry); // прорисовка лев окна
	wnd_commL =commander(wnd,pleft,dir,entry);	// прорисовка прав окна

	while (i != -1)
		switch(i=dir_navigation(wnd_commL,pleft)){	// РАБОТА В ЛЕВОМ ОКНЕ
		case 1: 		// нажатие ENT
			if ((pleft->y) == 3) {// переход выше по каталогу
				if(strlen(pleft->dir_name)>1)
				for (n=strlen(pleft->dir_name)-2;pleft->dir_name[n]!='/';n--)
					pleft->dir_name[n]=0;
				wnd_commL =commander(wnd,pleft,dir,entry);
				break;
			}
			while ((buff[pleft->x] = mvwinch(wnd_commL,pleft->y,pleft->x)) != ' ')
				(pleft->x)++;	// считывание имени в буфер
			(pleft->x)+=2;
			if ((mvwinch(wnd_commL,pleft->y,pleft->x)) == 'D')
			{			// переход на другой каталог
				buff[pleft->x-2]='/';
				strncat(pleft->dir_name,buff,pleft->x-1);
				wclear(wnd_commL);
				delwin(wnd_commL);
				pleft->x=0;pleft->y=4;
				wnd_commL =commander(wnd,pleft,dir,entry);
			}
			else {		// если выбран файл - открытие
				strncat(pleft->dir_name,buff,pleft->x-2);
				//wprintw(wnd_commL,pleft->dir_name);
				//wgetch(wnd_commL);
				wclear(wnd_commL);	// закрытие окон
				wrefresh(wnd_commL);
				delwin(wnd_commL);
				wclear(wnd_commR);
				wrefresh(wnd_commR);
				delwin(wnd_commR);
				wclear(wnd);
				wrefresh(wnd);
				delwin(wnd);

				if ((proc=fork()) == 0)	// запуск процесса-Редактора
				if (execl("text_editor"," ",
						pleft->dir_name,NULL) == -1)
					perror("exec failed");
				if (proc > 0) {	// возвращение удаленных окон
					waitpid(proc);
					wnd = newwin(0,0,0,0);
					box(wnd,'||','=');
					wrefresh(wnd);
					// стирание последней строки в dir_name
					for (n=strlen(pleft->dir_name)-1;pleft->dir_name[n]!='/';n--)
						pleft->dir_name[n]=0;
					pleft->x=0;pleft->y=4;
					wnd_commR = commander(wnd,pright,dir,entry);
					wnd_commL =commander(wnd,pleft,dir,entry);
					}
			}break;

		case 0:			// РАБОТА В ПРАВОМ ОКНЕ
			i=1;
			while ((i!=-1)&&(i!=0)) switch(i=dir_navigation(wnd_commR,pright)){
				case 1: 		// нажатие ENT
					if ((pright->y) == 3) {	// переход выше по каталогу
						if(strlen(pright->dir_name)>1)
						for (n=strlen(pright->dir_name)-2;pright->dir_name[n]!='/';n--)
							pright->dir_name[n]=0;
						wnd_commR =commander(wnd,pright,dir,entry);
						break;
					}
					while ((buff[pright->x] = mvwinch(wnd_commR,pright->y,pright->x)) != ' ')
						(pright->x)++;	// считывание имени в буфер
					(pright->x)+=2;
					if ((mvwinch(wnd_commR,pright->y,pright->x)) == 'D')
					{			// переход на другой каталог
						buff[pright->x-2]='/';
						strncat(pright->dir_name,buff,pright->x-1);
						wclear(wnd_commR);
						delwin(wnd_commR);
						pright->x=0;pright->y=4;
						wnd_commR =commander(wnd,pright,dir,entry);
					}
					else {		// если выбран файл - открытие
						strncat(pright->dir_name,buff,pright->x-2);
						wclear(wnd_commR);	// закрытие окон
						wrefresh(wnd_commR);
						delwin(wnd_commR);
						wclear(wnd_commL);
						wrefresh(wnd_commL);
						delwin(wnd_commL);
						wclear(wnd);
						wrefresh(wnd);
						delwin(wnd);

						if ((proc=fork()) == 0)	// запуск процесса-Редактора
						if (execl("text_editor"," ",
							pright->dir_name,NULL) == -1)
							perror("exec failed");
						if (proc > 0) {	// возвращение удаленных окон
							waitpid(proc);
						wnd = newwin(0,0,0,0);
						box(wnd,'||','=');
						wrefresh(wnd);
						// стирание последней строки в dir_name
						for (n=strlen(pright->dir_name)-1;pright->dir_name[n]!='/';n--)
							pright->dir_name[n]=0;
						pright->x=0;pright->y=4;
						wnd_commR = commander(wnd,pright,dir,entry);
						wnd_commL =commander(wnd,pleft,dir,entry);
						}
					}break;
			}break;
	}
	wclear(wnd_commL);
	wclear(wnd_commR);
	wclear(wnd);
	delwin(wnd_commL);
	delwin(wnd_commR);
	delwin(wnd);
	endwin();
	exit(EXIT_SUCCESS);

}

