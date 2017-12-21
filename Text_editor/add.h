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
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

void sig_winch(int signo);// обработчик сигнала SIGWINCH

void readFile(int fd,WINDOW *wnd);// буферизированное чтение из файла

void writeFile(int fd,WINDOW *wnd);// буферизированная запись в файл

#endif /* ADD_H_ */
