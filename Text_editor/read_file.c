/*
 * read_file.c
 * Буферизированное чтение из файла
 *  Created on: 21.12.2017
 *      Author: jake
 */

#include"add.h"


void readFile(int fd,WINDOW *wnd)
{
    unsigned char buffer[1];
    int bytes_read;

    if ((bytes_read = read(fd, buffer, 1)) !=0)
        {
    	wprintw(wnd,"%s", buffer);
        readFile(fd,wnd);
        }
}
