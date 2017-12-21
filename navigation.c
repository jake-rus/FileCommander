/*
 * navigation.c
 * Перемещение по каталогу
 *  Created on: 21.12.2017
 *      Author: jake
 */

#include "add.h"

int dir_navigation(WINDOW *wnd, comm *side)
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
