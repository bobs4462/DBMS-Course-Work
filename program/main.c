#define MAIN_H
#include <gui.h>
sqlite3 *db;

void vacant_time(sqlite3_context* ctx, int nargs, sqlite3_value** values);
void get_time(sqlite3_context* ctx, int nargs, sqlite3_value** values);

int main(void)
{
	if(sqlite3_open_v2("database/clinic.db", &db, SQLITE_OPEN_READWRITE, NULL)) {
		fprintf(stderr, "Ошибка открытия базы данных: %s\n", sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}
	sqlite3_create_function( db, "get_time", 1, SQLITE_UTF8, NULL, get_time, NULL, NULL);
	sqlite3_create_function( db, "vacant_time", 2, SQLITE_UTF8, NULL, vacant_time, NULL, NULL);
	sqlite3_stmt *stmt;
	char *sql = "PRAGMA foreign_keys = ON";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);

	setlocale(LC_ALL, "ru_RU.utf8");
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
	init_pair(6, COLOR_RED, COLOR_WHITE);

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

void get_time(sqlite3_context* ctx, int nargs, sqlite3_value** values)
{
	char *msg = calloc(100, 1);
	int minutes = sqlite3_value_int(values[0]);
	sprintf(msg, "%s%d:%s%d",
			minutes / 60 > 9 ? "" : "0",
			minutes / 60,
			minutes % 60 ? "" : "0",
			minutes % 60);
	sqlite3_result_text(ctx, msg, strlen(msg), SQLITE_STATIC);
}

void vacant_time(sqlite3_context* ctx, int nargs, sqlite3_value** values)
{
	char **vacant_times = NULL;
	char **busy_times = NULL;
	int i = 0, j = 0, flag = 0;
	sqlite3_stmt *stmt;
	char *sql = "SELECT strftime('%H:%M', recdatetime) FROM appointment WHERE tabid = ? \
		   AND strftime('%Y-%m-%d', recdatetime) = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, sqlite3_value_int(values[0]));
	sqlite3_bind_text(stmt, 2, sqlite3_value_text(values[1]), -1, SQLITE_STATIC);
	for (i = 0; sqlite3_step(stmt) != SQLITE_DONE; ++i) {
		busy_times = realloc(busy_times, sizeof(char *) * (i + 1));
		busy_times[i] = calloc(100, 1);
		strcpy(busy_times[i], sqlite3_column_text(stmt, 0));
	}
	sqlite3_finalize(stmt);
	sql = "SELECT get_time(shiftst), get_time(shiftend), get_time(break) FROM \
		   timetable where tabid = ? AND daynum  = case \
		   when CAST(strftime('%w', ?) as INTEGER) \
		   then CAST(strftime('%w', ?) as INTEGER) \
		   else 7 end;";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, sqlite3_value_int(values[0]));
	sqlite3_bind_text(stmt, 2, sqlite3_value_text(values[1]), -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, sqlite3_value_text(values[1]), -1, SQLITE_STATIC);
	sqlite3_step(stmt);
	char start[50], end[50];
	strcpy(start, sqlite3_column_text(stmt, 0));
	strcpy(end, sqlite3_column_text(stmt, 1));
	busy_times = realloc(busy_times, sizeof(char *) * (i + 2));
	busy_times[i] = calloc(50, 1);
	strcpy(busy_times[i], sqlite3_column_text(stmt, 2));
	busy_times[i + 1] = NULL;

	sqlite3_finalize(stmt);
	int w = 0;
	sql = "SELECT strftime('%H:%M', ?, ?)";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, start, -1, SQLITE_STATIC);
	char mod[20] = {0};
	sprintf(mod, "+%d minutes", w);
	int ok = sqlite3_bind_text(stmt, 2, mod, -1, SQLITE_STATIC);
	if (ok != SQLITE_OK) {
		printw("ERROR %s", sqlite3_errmsg(db));
		refresh();
		exit(1);
	}
	sqlite3_step(stmt);

	for (j = 0, i = 0; strcmp(sqlite3_column_text(stmt, 0), end) < 0; ++j) {
		for (int k = 0; busy_times[k]; ++k)	
			if (!strcmp(busy_times[k], sqlite3_column_text(stmt, 0)))
				flag = 1;	
		if (!flag) {
			vacant_times = realloc(vacant_times, sizeof(char *) * (i + 2));
			vacant_times[i] = calloc(50, 1);	
			strcpy(vacant_times[i], sqlite3_column_text(stmt, 0));
			vacant_times[++i] = NULL;
		}
		flag = 0;
		sqlite3_reset(stmt);
		sqlite3_bind_text(stmt, 1, start, -1, SQLITE_STATIC);
		sprintf(mod, "+%d minutes", ++w * 30);
		sqlite3_bind_text(stmt, 2, mod, -1, SQLITE_STATIC);
		sqlite3_step(stmt);
	}
	sqlite3_result_blob(ctx, vacant_times, (i + 1) * sizeof(char *), SQLITE_STATIC);
	sqlite3_finalize(stmt);
}
