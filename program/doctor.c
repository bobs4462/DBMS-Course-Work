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
		switch (show_menu(options, 8, "Выберите действие")) {
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
				cure_create(regid, tabid);
				break;
			case 4:
				receipt_issue(tabid);
				break;
			case 5:
				sick_leave_issue(regid);
				break;
			default:
				exit(0);
		}

	delwin(pat->sub);
	delwin(pat->win);
	del_panel(pat->pan);
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
	char **fields = get_input("Назначение анализов", descriptions, 1, 5, 35);
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

	char **fields = get_input("Заполните поля", descriptions, 2, 4, 40);
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
	free(fields[0]);
	free(fields[1]);
	free(fields);
	return 0;
}

int cure_create(int regid, int tabid)
{
	sqlite3_stmt *stmt;
	message_box("Выберите карту пациента", NULL, 5, COLS - 50, 2, 25);	
	int cardid = medical_cards(regid, 1);

	char *descriptions[] = {
		"Заболевание",
		"Лечение",
	};

	char **fields = get_input("Заполните поля", descriptions, 2, 5, 50);
	char *sql = "INSERT INTO treatment(tabid, cardid, illness, treatment)\
				 values(?, ?, ?, ?)";
	char msg[100];
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);
	sqlite3_bind_int(stmt, 2, cardid);
	sqlite3_bind_text(stmt, 3, fields[0], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, fields[1], -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		sql = "SELECT last_insert_rowid()"; 
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_step(stmt);
		sprintf(msg, "Лечение № %d успешно добавлено", sqlite3_column_int(stmt, 0));
		message_box(msg, "Создание Лечения", -1, -1, 4, 30);
	}
	sqlite3_finalize(stmt);
	free(fields[0]);
	free(fields[1]);
	free(fields);

	return 0;
}
int receipt_issue(int tabid)
{
	sqlite3_stmt *stmt;

	char *descriptions[] = {
		"Лекарства",
	};

	char **fields = get_input("Заполните поля", descriptions, 1, 5, 50);
	char *sql = "INSERT INTO receipt(tabid, medicine)\
				 values(?, ?)";
	char msg[100];
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);
	sqlite3_bind_text(stmt, 2, fields[0], -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		sql = "SELECT last_insert_rowid()"; 
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_step(stmt);
		sprintf(msg, "Рецепт № %d успешно добавлен", sqlite3_column_int(stmt, 0));
		message_box(msg, "Создание рецепта", -1, -1, 4, 30);
	}
	sqlite3_finalize(stmt);
	free(fields[0]);
	free(fields);
	return 0;
}
int sick_leave_issue(int regid)
{
	sqlite3_stmt *stmt;

	char *descriptions[] = {
		"Дата начала",
		"Дата окончания",
		"Место назначения"
	};

	char **fields = get_input("Создание больничного", descriptions, 3, 1, 30);
	char *sql = "INSERT INTO sickleave(regid, stdate, endate, destn)\
				 values(?, ?, ?, ?)";
	char msg[100];
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);
	sqlite3_bind_text(stmt, 2, fields[0], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, fields[1], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, fields[2], -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		sql = "SELECT last_insert_rowid()"; 
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_step(stmt);
		sprintf(msg, "Больничный № %d успешно создан", sqlite3_column_int(stmt, 0));
		message_box(msg, "Выписка листа нетрудоспособности", -1, -1, 4, 30);
	}
	sqlite3_finalize(stmt);
	free(fields[0]);
	free(fields[1]);
	free(fields[2]);
	free(fields);
	return 0;
}
