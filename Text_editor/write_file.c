/*
 * write_file.c
 * Буферизированная запись в файл
 *  Created on: 21.12.2017
 *      Author: jake
 */

#include"add.h"

void writeFile(int fd,WINDOW *wnd)
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


