/*
 * add.h
 *
 *  Created on: 21.12.2017
 *      Author: jake
 */

#ifndef ADD_H_
#define ADD_H_

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

void sig_winch(int signo);
int selectLine(WINDOW *wnd,int y);
WINDOW *commander(WINDOW *fwnd,comm *side, DIR *dir,struct dirent *entry);
int dir_navigation(WINDOW *wnd, comm *side);


#endif /* ADD_H_ */
