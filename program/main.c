#define MAIN_H
#include <sqlite3.h>
#include <gui.h>

int main(void)
{
	sqlite3 *clinicdb;
	if(sqlite3_open("clinic.db", &clinicdb)) {
		fprintf(stderr, "Ошибка открытия базы данных: %s\n", sqlite3_errmsg(clinicdb));
		exit(EXIT_FAILURE);
	}
	setlocale(LC_ALL, "");
	char *credentials[2];
	initscr();
	cbreak();
	noecho();
	user_t usertype = login(credentials);
	switch(usertype) {
		case UNKNOWN :
			mvprintw(LINES / 2, COLS / 2 - strlen(LOG_INCORRECT) / 4, LOG_INCORRECT);
			refresh();
			getch();
			goto EXIT;
		case PATIENT:
			patient_interface(clinicdb);
		case DOCTOR:
			doctor_interface(clinicdb);
		case REGISTRY:
			registry_interface(clinicdb);
	}
EXIT:
	nocbreak();
	echo();
	endwin();
	return 0;

}
