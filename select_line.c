/*
 * select_line.c
 * Выделение строчки
 *  Created on: 21.12.2017
 *      Author: jake
 */

#include "add.h"

int selectLine(WINDOW *wnd,int y)
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
