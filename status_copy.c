/*
 * status_copy.c
 *	Панель состояния процедуры копирования
 *  Created on: 08.01.2018
 *      Author: jake
 */


#include "add.h"

void *status_bar(void *args)
{
	WINDOW* wnd_stat;
	copy_struct * names = (copy_struct *)args;
	long int percent=0;
	long int size_sr,curr_dt,curr_sr;
	FILE *fsr,*fdt;

	wnd_stat = derwin(names->fwnd,1,names->fwnd->_maxx/2,2,names->fwnd->_maxx/2-1);

	pthread_mutex_lock(&mutexSr);
	fsr=fopen(names->file_nameS, "r");
	pthread_mutex_unlock(&mutexSr);

		if (fsr == NULL)
			perror("fileopen SR error");else
			{// Проверка размера источника
				pthread_mutex_lock(&mutexSr);
				curr_sr = ftell(fsr);
				fseek(fsr,0,SEEK_END);
				size_sr = ftell(fsr);
				fseek(fsr,curr_sr,SEEK_SET);
				pthread_mutex_unlock(&mutexSr);

				pthread_mutex_lock(&mutexDt);
				fdt=fopen(names->file_nameD, "r");
				pthread_mutex_unlock(&mutexDt);

				if (fdt==NULL)
					perror("fileopen DT error"); else
					{// расчет процента копирования
						while (percent < 100) {
							wclear(wnd_stat);
							wrefresh(wnd_stat);
							pthread_mutex_lock(&mutexDt);
							curr_dt = ftell(fdt);
							fseek(fdt,0,SEEK_END);
							percent = 100*ftell(fdt)/size_sr;
							fseek(fdt,curr_dt,SEEK_SET);
							pthread_mutex_unlock(&mutexDt);
							wprintw(wnd_stat, "copy - %ld",percent);
							wrefresh(wnd_stat);
						}
		  // Закрываем файлы
						fclose(fdt);
						fclose(fsr);
					}
			}
	wclear(wnd_stat);
	wrefresh(wnd_stat);
	wrefresh(wnd_stat);
	delwin(wnd_stat);
	pthread_exit(0);
}
