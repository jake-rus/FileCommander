/*
 * File Commander
 *
 *  Created on: 18.12.2017
 *      Author: jake
 */
#include "add.h"

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
				if (execl("Text_editor/text_editor"," ",
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
						if (execl("Text_editor/text_editor"," ",
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

