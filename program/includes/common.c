#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <form.h>
#include <menu.h>
#include <panel.h>
extern sqlite3 *db;
enum colors {GREEN = 1, RED, CYAN, MAGENTA, BLUE};

int message_box(const char *text, char *header) 
{
	WINDOW *message_win, *message_sub;

	PANEL *message_pan;
	message_win = newwin(20, 50, LINES / 2 - 5, COLS / 2 - 25);
	message_sub = derwin(message_win, 10, 56, 3, 2);
	message_pan = new_panel(message_win);
	
	mvwaddch(message_win, 2, 0, ACS_LTEE);
	mvwhline(message_win, 2, 1, ACS_HLINE, 48);
	mvwaddch(message_win, 2, 49, ACS_RTEE);
	mvwprintw(message_win, 1, 2, header);
	mvwprintw(message_sub, 0, 0, text);
	box(message_win, 0, 0);
	update_panels();
	doupdate();
	wgetch(message_win);
	del_panel(message_pan);
	delwin(message_sub);
	delwin(message_win);
	update_panels();
	doupdate();
}
