/*
 * copy_file.c
 *	Копирование файла между директориями, открытыми в окнах
 *  Created on: 07.01.2018
 *      Author: jake
 */

#include "add.h"

#define SIZE_OF_BUFF 100 // копируем по 100 байт

void *copy_file(void *args)
{
	copy_struct * names = (copy_struct *)args;
	char buff[SIZE_OF_BUFF]; // буффер
	size_t n=1; // количество считанных байт
	FILE *fsr, *fdt;

	pthread_mutex_lock(&mutexSr);
	fsr=fopen(names->file_nameS, "rb");
	pthread_mutex_unlock(&mutexSr);

	if (fsr == NULL)
		perror("fileopen error"); else {// Откр файл-источник

			pthread_mutex_lock(&mutexDt);
			fdt=fopen(names->file_nameD, "wb");
			pthread_mutex_unlock(&mutexDt);

			if (fdt == NULL)
				perror("fileopen error"); else {// Откр файл-приёмник
					while (n != 0){
						pthread_mutex_lock(&mutexSr);
						n=fread(buff, sizeof(char), SIZE_OF_BUFF, fsr);
						pthread_mutex_unlock(&mutexSr);
						pthread_mutex_lock(&mutexDt);
						fwrite(buff, sizeof(char), n, fdt);
						pthread_mutex_unlock(&mutexDt);
						usleep(500); // задержка для наглядности процесса
					}
	  // Закрываем файлы
					fclose(fdt);
			}
			fclose(fsr);
	}

	pthread_exit(0);
}
