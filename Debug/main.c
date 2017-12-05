/*
 * main.c
 *
 *  Created on: 21 нояб. 2017 г.
 *      Author: jake
 */
    #include <stdlib.h>  
    #include <curses>  
      
    int main(void)  
    {  
        initscr();  
        printw("Hi!\n");  
        refresh();  
        getch();  
        endwin();  
      
        exit(0);  
    }  

