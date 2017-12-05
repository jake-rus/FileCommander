#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <panel.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>

void readFile(int fd,WINDOW *wnd) 		// буферизированное чтение из файла
{
    unsigned char buffer[1];
    int bytes_read;

    if ((bytes_read = read(fd, buffer, 1)) !=0)
        {
    	wprintw(wnd,"%s", buffer);
        readFile(fd,wnd);
        }
}

void writeFile(int fd,WINDOW *wnd) 		// буферизированная запись в файл
{
	int x=0,y=0;
	int buffer[1];

	for (y=0;y<=wnd->_maxy; y++){
	for (x=0;x<=wnd->_maxx-1; x++){
    *buffer = mvwinch(wnd,y,x);
	write(fd, buffer,1);};
	buffer[0]=0;
	write(fd,buffer,1);}

}

int editor(WINDOW *fwnd) 	// ТЕКСТОВЫЙ РЕДАКТОР
{
	WINDOW * wnd_editorMenu; // Меню редактора
	WINDOW * wnd_editor;	// окно редактора
	short highEditorMenu=10;
	int ch=0,sym,c,x=0,y=0;
	char file_name[255];
	int filedesc = 0;

	wnd_editorMenu = derwin(fwnd,highEditorMenu,fwnd->_maxx/2-2,1,1);
	wclear(wnd_editorMenu);
	init_pair(2,COLOR_RED, COLOR_CYAN);
	wbkgd(wnd_editorMenu,COLOR_PAIR(2));
	wattron(wnd_editorMenu,A_BOLD);
	wrefresh(wnd_editorMenu);
	wnd_editor = derwin (fwnd,fwnd->_maxy-highEditorMenu-2,fwnd->_maxx/2-2,wnd_editorMenu->_maxy+2,1);
	wclear(wnd_editor);
	wprintw(wnd_editorMenu,"1.Read file\n2.Edit/Save file\n3.Close file\n4.Exit\n\nFriend, select action with file:\n"); // печать в подокне
	wrefresh(wnd_editorMenu);
	keypad(wnd_editor, TRUE);
L: 	echo();
	ch = getch();
	switch(ch)
		{
		case '1':wprintw(wnd_editorMenu,"file:");				// чтение файла
				curs_set(1);
				init_pair(1,COLOR_RED, COLOR_BLUE);
				wbkgd(wnd_editorMenu,COLOR_PAIR(1));
				wrefresh(wnd_editorMenu);
				wgetnstr(wnd_editorMenu,file_name,256);
				noecho(); // отключение отображения символов
				wrefresh(wnd_editorMenu);
				if ((filedesc = open(file_name,O_RDONLY)) != -1)
				readFile(filedesc,wnd_editor);
				wrefresh (wnd_editor);
				goto L;

		case '2':										// редактирование и сохранение файла
				curs_set(1);
				keypad(wnd_editor, TRUE);
				if ((filedesc != -1) && (filedesc != 0)){
					wmove(wnd_editor,0,0);
					x=0;y=0;
		   loop:    c=wgetch(wnd_editor);
				    switch(c)
				    {
				        case KEY_RIGHT:
				            x++;
				            wmove(wnd_editor,y,x);
				            wrefresh(wnd_editor);
				            goto loop;

				        case KEY_LEFT:
				            x--;
				            wmove(wnd_editor,y,x);
				            wrefresh(wnd_editor);
				            goto loop;

				        case KEY_UP:
				            y--;
				            wmove(wnd_editor,y,x);
				            wrefresh(wnd_editor);
				            goto loop;

				        case KEY_DOWN:
				            y++;
				            wmove(wnd_editor,y,x);
				            wrefresh(wnd_editor);
				            goto loop;

				        case 27: break;

				        default:
				            wprintw(wnd_editor,"%c",c);
				            wrefresh(wnd_editor);
				            goto loop;
				    }
				writeFile(filedesc,wnd_editor);
				}
				curs_set(0);
				goto L;

		case '3': if (filedesc != 0 && filedesc != -1){ 			// закрытие файла
				close(filedesc);
				filedesc=0;
				wbkgd(wnd_editorMenu,COLOR_PAIR(2));				//очистка редактора
				werase(wnd_editor);
				wrefresh (wnd_editor);
				wrefresh (wnd_editorMenu);}
				goto L;

		case '4': break;
		default: goto L;
		}
	wrefresh(wnd_editor);
	delwin(wnd_editorMenu);
	delwin(wnd_editor);
	return 0;
}
