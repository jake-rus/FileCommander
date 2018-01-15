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
#include <pthread.h>

typedef struct {			// структура лев/прав окна менеджера
	char id;		// идентификатор лев(0)/прав(1) окна менеджера
	char dir_name[255]; // имя каталога
	int x,y;			// координаты курсора
} comm;

typedef struct {	// структура для передачи имен потоку-копиру
	char id;		// идентификатор окна-инициатора копирования
	char *file_nameS; // источник
	char *file_nameD; // приемник
	WINDOW *fwnd;	// указатель на окно назначения копирования
	} copy_struct;

pthread_mutex_t mutexSr; // мьютекс на работу с файлом 1
pthread_mutex_t mutexDt; // мьютекс на работу с файлом 2

void sig_winch(int signo);
int selectLine(WINDOW *wnd,int y);
WINDOW *commander(WINDOW *fwnd,comm *side);
int dir_navigation(WINDOW *wnd, comm *side);
void *copy_file(void *args);
void * status_bar(void *args);


#endif /* ADD_H_ */
