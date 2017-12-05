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

int selectLine(WINDOW *wnd,int y) // 	Выделение строчки файла
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

int commander(WINDOW *fwnd, _side side)		// ФАЙЛОВЫЙ МЕНЕДЖЕР
{
	WINDOW *wnd_commander;
	struct dirent *entry;
	DIR *dir;
	int n;
	char dir_name[255]="/";
	int x=0,y=2,c;
	char d[]="DIR";
	char f[]="file";

loop:switch (side){ // варианты правого\левого менеджеров файлов
	case 1: wnd_commander = derwin(fwnd,fwnd->_maxy-2,fwnd->_maxx/2-2,1,fwnd->_maxx/2+1);break;
	default: wnd_commander = derwin(fwnd,fwnd->_maxy-2,fwnd->_maxx/2-2,1,1);break;}
	wclear(wnd_commander);	// очистка окна менеджера
	wattron(wnd_commander,A_STANDOUT);
	mvwprintw(wnd_commander, 0,0,"TAB - switch R/L, SPACE - select, ENT - open\n");
	wattroff(wnd_commander,A_STANDOUT);
	wattron(wnd_commander,A_UNDERLINE);
	wprintw(wnd_commander, "Dir: %s", dir_name);
	wattroff(wnd_commander,A_UNDERLINE);

    curs_set(1);
    echo();
    wrefresh(wnd_commander);
    //wgetnstr(wnd_commander,dir_name,256); // ввод каталога

    dir = opendir(dir_name);	// проверка окрытия каталога
    if (!dir) {perror("diropen"); delwin(wnd_commander);goto loop;}
    else {
    while ( (entry = readdir(dir)) != NULL) {
        wprintw(wnd_commander,"%s [%s]\n",
            entry->d_name, (entry->d_type==DT_DIR)?d:f);
    };
    curs_set(1);
    keypad(wnd_commander, TRUE);
    wrefresh(wnd_commander);
    noecho();
    cbreak();
loop2:c=wgetch(wnd_commander);		// управление в файловом менеджере
    	switch(c)
    	{
    		case KEY_UP:
				y--;
				wmove(wnd_commander,y,x);
				wrefresh(wnd_commander);
				goto loop2;

    		case KEY_DOWN:
				y++;
				wmove(wnd_commander,y,x);
				wrefresh(wnd_commander);
				goto loop2;

    		case ' ':
    			selectLine(wnd_commander,y); // выделение строчки
    			wrefresh(wnd_commander);
    			goto loop2;

    		case 9:							// переключение между сторонами
    			delwin(wnd_commander);
    			closedir(dir);
    			(side)? commander(fwnd, left): commander(fwnd, right);break;

    		case 10:
				editor(fwnd); // вызов текстового редактора
				break;

    		case 27:
    			exit(EXIT_SUCCESS);

    		default:
    			goto loop2;
    	}

		}

	return 0;
}
