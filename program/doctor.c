#include <doctor.h>

void doctor_interface(int tabid)
{
	char mask[1000];
	char *s;
	sqlite3_stmt *stmt;

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
		patients = realloc(patients, sizeof(char *) * (i + 2));
		patients[i] = malloc(500);
		sprintf(patients[i], "%4d %-s", 
				sqlite3_column_int(stmt, 0),
				sqlite3_column_text(stmt, 1));
	}
	if (!i) {
		message_box("Пациентов не найдено", "УПС!!!", -1, -1, 3, 25);
		goto SEARCH;
	}

	patients[i] = NULL;
	int r = show_menu(patients, i + 1, "Выберите пациента!");
	sqlite3_reset(stmt);
	for (int j = 0; (sqlite3_step(stmt) != SQLITE_DONE) && (j < r); ++j);
	int regid = sqlite3_column_int(stmt, 0);
	struct win_pan *pat = patient_info(regid);
	char *options[] = {
		"Просмотр карт пациента",
		"Назначение анализов",
		"Внести результат анализов",
		"Назначение лечения",
		"Выписка рецепта",
		"Выдача листка нетрудоспособности",
		"Выход",
		NULL
	};
	while(TRUE)
		switch (show_menu(options, 6, "Выберите действие")) {
			case 0:
				medical_cards(regid, 0);
				break;
			case 1:
				annalysis_create(tabid, regid);
				break;
			case 2:
				annalysis_update();
				break;
			case 3:
				cure_create(regid);
				break;
			case 4:
				receipt_issue(regid);
				break;
			case 5:
				sick_leave_issue(regid);
				break;
			default:
				exit(0);
		}

	refresh();
	getch();
}

int annalysis_create(int tabid, int regid)
{
	sqlite3_stmt *stmt;
	message_box("Выберите карту пациента", NULL, 5, COLS - 50, 2, 25);	
	int cardid = medical_cards(regid, 1);
	char *descriptions[] = {
		"Вид анализов",
	};

	char msg[100];

	//char today[21];
	//time_t now = time(NULL);
	//strftime(today, 20, "%Y-%m-%d", localtime(&now));
	char **fields = get_input("Назначение анализов", descriptions, 1, 3, 30);
	char *sql = "INSERT INTO analysis(tabid, cardid, type) values(?, ?, ?)";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);
	sqlite3_bind_int(stmt, 2, cardid);
	sqlite3_bind_text(stmt, 3, fields[0], -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		sql = "SELECT last_insert_rowid()"; 
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_step(stmt);
		sprintf(msg, "Анализ № %d успешно назначен", sqlite3_column_int(stmt, 0));
		message_box(msg, "Номер анализа", -1, -1, 4, 30);
	}

	sqlite3_finalize(stmt);
	refresh();
	free(fields[0]);
	free(fields);
	return 0;
}

int annalysis_update(void)
{
	sqlite3_stmt *stmt;
	char *descriptions[] = {
		"Номер анализов",
		"Результат анализов",
	};

	char **fields = get_input("Заполинте поля", descriptions, 2, 3, 30);
	char *sql = "UPDATE analysis SET result = ? where anid = ?";
	char msg[100];
	int anid = atoi(fields[0]);
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 2, anid);
	sqlite3_bind_text(stmt, 1, fields[1], -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sprintf(msg, "Анализ № %d успешно обновлен", anid);
		message_box(msg, "Обновление анализа", -1, -1, 4, 30);
	}
	sqlite3_finalize(stmt);
	return 0;
}

int cure_create(int regid)
{
	return 0;
}
int receipt_issue(int regid)
{
	return 0;
}
int sick_leave_issue(int regid)
{
	return 0;
}
