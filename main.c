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
	pthread_t tidCopy; //идентификатор потока-копира
	pthread_t tidStatus; //идентификатор потока-панели
	copy_struct names_copy;
	copy_struct *arg_copy = &names_copy; //указатель на имена для копирования
	pthread_mutex_init(&mutexSr, NULL);
	pthread_mutex_init(&mutexDt, NULL);
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
	wnd_commR = commander(wnd,pright); // прорисовка лев окна
	wnd_commL =commander(wnd,pleft);	// прорисовка прав окна

	while (i != -1)
		switch(i=dir_navigation(wnd_commL,pleft)){	// РАБОТА В ЛЕВОМ ОКНЕ
		case 1: 		// нажатие ENT
			if ((pleft->y) == 3) {// переход выше по каталогу
				if(strlen(pleft->dir_name)>1)
				for (n=strlen(pleft->dir_name)-2;pleft->dir_name[n]!='/';n--)
					pleft->dir_name[n]=0;
				wnd_commL =commander(wnd,pleft);
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
				wnd_commL =commander(wnd,pleft);
			}
			else {		// если выбран файл - открытие
				strncat(pleft->dir_name,buff,pleft->x-2);
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
				if (execl("/home/jake/workspace/File_Editor/Debug/File_Editor"," ",
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
					wnd_commR = commander(wnd,pright);
					wnd_commL =commander(wnd,pleft);
					}
			}break;
		case 2:			// КОПИРОВАНИЕ ФАЙЛА СЛЕВА -> НАПРАВО
			if ((pleft->y) > 4) {// работает только с содержимым каталога
				while ((buff[pleft->x] = mvwinch(wnd_commL,pleft->y,pleft->x)) != ' ')
					(pleft->x)++;	// считывание имени в буфер
				(pleft->x)+=2;
				if ((mvwinch(wnd_commL,pleft->y,pleft->x)) == 'f')
				{	// составляем полное имя файла для обеих сторон
					strncat(pleft->dir_name,buff,pleft->x-2);
					strncat(pright->dir_name,buff,pleft->x-2);
					// составляем указатели на имена в структуру аргумента
					names_copy.file_nameS = pleft->dir_name;
					names_copy.file_nameD = pright->dir_name;
					names_copy.fwnd = wnd_commR;
					pthread_create(&tidCopy,NULL,copy_file,arg_copy); // копирование файла
					pthread_create(&tidStatus,NULL,status_bar,arg_copy);// панель состояния копирования
					// стирание последней строки в dir_name слева и справа
					pthread_join(tidCopy, NULL); // завершение потока-Копира
					pthread_join(tidStatus, NULL); // завершение потока-статуса копирования
					for (n=strlen(pleft->dir_name)-1;pleft->dir_name[n]!='/';n--)
						pleft->dir_name[n]=0;
					for (n=strlen(pright->dir_name)-1;pright->dir_name[n]!='/';n--)
						pright->dir_name[n]=0;
					pleft->x=0;
					// обновление окон после копирования
					wclear(wnd_commL);	// закрытие окон
					wrefresh(wnd_commL);
					delwin(wnd_commL);
					wclear(wnd_commR);
					wrefresh(wnd_commR);
					delwin(wnd_commR);
					wclear(wnd);
					wrefresh(wnd);
					wnd_commR = commander(wnd,pright);
					wnd_commL = commander(wnd,pleft);
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
						wnd_commR =commander(wnd,pright);
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
						wnd_commR =commander(wnd,pright);
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
						if (execl("/home/jake/workspace/File_Editor/Debug/File_Editor"," ",
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
						wnd_commR = commander(wnd,pright);
						wnd_commL =commander(wnd,pleft);
						}
					}break;
				case 2:		// КОПИРОВАНИЕ ФАЙЛА СПРАВА -> НАЛЕВО
					if ((pright->y) > 4) {// работает только с содержимым каталога
						while ((buff[pright->x] = mvwinch(wnd_commR,pright->y,pright->x)) != ' ')
							(pright->x)++;	// считывание имени в буфер
						(pright->x)+=2;
						if ((mvwinch(wnd_commR,pright->y,pright->x)) == 'f')
						{	// составляем полное имя файла для обеих сторон
							strncat(pright->dir_name,buff,pright->x-2);
							strncat(pleft->dir_name,buff,pright->x-2);
							// составляем указатели на имена в структуру аргумента
							names_copy.file_nameS = pright->dir_name;
							names_copy.file_nameD = pleft->dir_name;
							names_copy.fwnd = wnd_commL;
							pthread_create(&tidCopy,NULL,copy_file,arg_copy); // копирование файла
							pthread_create(&tidStatus,NULL,status_bar,arg_copy);// панель состояния копирования
							// стирание последней строки в dir_name слева и справа
							pthread_join(tidCopy, NULL); // завершение потока-Копира
							pthread_join(tidStatus, NULL); // завершение потока-статуса копирования
							for (n=strlen(pright->dir_name)-1;pright->dir_name[n]!='/';n--)
								pright->dir_name[n]=0;
							for (n=strlen(pleft->dir_name)-1;pleft->dir_name[n]!='/';n--)
								pleft->dir_name[n]=0;
							pright->x=0;
							// обновление окон после копирования
							wclear(wnd_commL);	// закрытие окон
							wrefresh(wnd_commL);
							delwin(wnd_commL);
							wclear(wnd_commR);
							wrefresh(wnd_commR);
							delwin(wnd_commR);
							wclear(wnd);
							wrefresh(wnd);
							wnd_commR = commander(wnd,pright);
							wnd_commL = commander(wnd,pleft);
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
	pthread_mutex_destroy(&mutexSr);
	pthread_mutex_destroy(&mutexDt);

	exit(EXIT_SUCCESS);

}
