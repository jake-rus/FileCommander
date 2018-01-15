/*
 * dir_reader.c
 * Чтение каталога (функция commander)
 *  Created on: 21.12.2017
 *      Author: jake
 */

#include "add.h"

WINDOW *commander(WINDOW *fwnd,comm *side)
{
	WINDOW *wnd_commander;
	DIR *dir=NULL;			// указатель на каталог
	struct dirent *entry; 	// записи в структуре каталога

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
