#include <registry.h>
void registry_interface(void)
{
	char *options[] = {
		"Просмотр карт пациентов",
		"Добавить нового пациента",
		"Просмотр расписания врачей",
		"Создать запись к врачу",
		"Выход",
		NULL
	};
	while (TRUE) {
		switch (show_menu(options, 6, "Доступные действия", 4, -1)) {
			case 0:
				medcard_search();
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			default:
				goto EXIT;
		}
	}
EXIT:
	return;
}

int medcard_search(void) 
{
	int choice;
	sqlite3_stmt *stmt;
	char *s, mask[500];
	char *sql = "SELECT regid, fio FROM patient WHERE fio LIKE ?";
SEARCH:
	sprintf(mask, "%%%s%%", s = search("Поиск пациента"));
	free(s);
	refresh();
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 1, mask, -1, SQLITE_STATIC);
	char **patients = NULL;

	int i = 0;
	for (i = 0; sqlite3_step(stmt) != SQLITE_DONE; ++i) {
		patients = realloc(patients, sizeof(char *) * (i + 2));;
		patients[i] = malloc(500);
		sprintf(patients[i], "%4d %-s", 
				sqlite3_column_int(stmt, 0),
				sqlite3_column_text(stmt, 1));
	}
	if (!i) {
		message_box("Пациентов не найдено", "УПС!!!", -1, -1, 3, 25, 0);
		goto SEARCH;
	}

	patients[i] = NULL;
	int r = show_menu(patients, i + 1, "Выберите пациента!", -1, -1);
	sqlite3_reset(stmt);
	for (int j = 0; (sqlite3_step(stmt) != SQLITE_DONE) && (j < r); ++j);
	int regid = sqlite3_column_int(stmt, 0);
	struct win_pan *pat = patient_info(regid);

	char *options[] = {
		"Просмотр карт пацента",
		"Создать новую медкарту",
		NULL
	};
}
