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
	int regid;
	initscr();
	cbreak();
	noecho();
	start_color();
	user_t usertype = login(&regid);
		
	switch(usertype) {
		case PATIENT:
			patient_interface(regid);
		case DOCTOR:
			doctor_interface(regid);
		case REGISTRY:
			registry_interface(regid);
	}
EXIT:
	nocbreak();
	echo();
	endwin();
	return 0;

}
