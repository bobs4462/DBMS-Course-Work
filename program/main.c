#define MAIN_H
#include <gui.h>
sqlite3 *db;

int main(void)
{
	if(sqlite3_open_v2("database/clinic.db", &db, SQLITE_OPEN_READWRITE, NULL)) {
		fprintf(stderr, "Ошибка открытия базы данных: %s\n", sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}
	setlocale(LC_ALL, "");
	int regid;
	initscr();
	cbreak();
	noecho();
	start_color();

	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_CYAN, COLOR_BLACK);
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(5, COLOR_BLUE, COLOR_BLACK);

	keypad(stdscr, TRUE);
	curs_set(0);
	user_t usertype = login(&regid);
		
	switch(usertype) {
		case PATIENT:
			patient_interface(regid);
			break;
		case DOCTOR:
			doctor_interface(regid);
			break;
		case REGISTRY:
			registry_interface(regid);
			break;
		case UNKNOWN:
			mvprintw(LINES / 2, (COLS - strlen(LOG_INCORRECT) / 2) / 2, LOG_INCORRECT);
			refresh();
			getchar();
			break;
	}
EXIT:
	nocbreak();
	echo();
	endwin();
	return 0;

}
