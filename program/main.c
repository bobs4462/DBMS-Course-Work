#define MAIN_H
#include <sqlite3.h>
#include <gui.h>
sqlite3 *db;

int main(void)
{
	if(sqlite3_open_v2("clinic.db", &db, SQLITE_OPEN_READWRITE, NULL)) {
		fprintf(stderr, "Ошибка открытия базы данных: %s\n", sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}
	setlocale(LC_ALL, "");
	char *credentials[2];
	initscr();
	cbreak();
	noecho();
	user_t usertype = login();
		
	switch(usertype) {
		case UNKNOWN :
			mvprintw(LINES / 2, COLS / 2 - strlen(LOG_INCORRECT) / 4, LOG_INCORRECT);
			refresh();
			getch();
			goto EXIT;
		case PATIENT:
			patient_interface();
		case DOCTOR:
			doctor_interface();
		case REGISTRY:
			registry_interface();
	}
EXIT:
	nocbreak();
	echo();
	endwin();
	return 0;

}
