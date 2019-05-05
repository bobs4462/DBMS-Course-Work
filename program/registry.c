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
	int regid = 0;
	while (TRUE) {
		switch (show_menu(options, 6, "Доступные действия", 4, -1)) {
			case 0:
				medcard_search();
				break;
			case 1:
				create_patient();
				break;
			case 2:
				timetable(0);
				break;
			case 3:
				regid = patient_search();
				appointment(regid);
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
	switch (show_menu(options, 3, "Выберите действие", -1, -1)) {
		case 0:
			medical_cards(regid, 0);
			break;
		case 1:
			card_create(regid);
			break;
	}
}
int card_create(int regid)
{
	sqlite3_stmt *stmt;
	char *sql = "INSERT INTO medcard(regid, type) VALUES(?, ?)";
	char **type = get_input("Введите тип карты", (char*[]){""}, 1, 1, 40, NULL);
	char msg[500];
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 2, type[0], -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 1, regid);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		sql = "SELECT last_insert_rowid()"; 
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL); 
		sqlite3_step(stmt);
		sprintf(msg, "Карта № %d успешно создана", sqlite3_column_int(stmt, 0));
		message_box(msg, "Создание карты", -1, -1, 3, 40, 0);
	}
	else {
		for (int i = 0; i < 9; ++i) 
			if (sqlite3_step(stmt) == SQLITE_DONE) {
				mvprintw(1,1, "ВСё гуд");
				break;
			}
			else
				mvprintw(1,1, sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);
	return 0;
}

int create_patient(void)
{
	sqlite3_stmt *stmt;
	char *sql = "INSERT INTO patient(fio, insurcomp, icontract, birthdate, gender, address, pnumber) VALUES(?, ?, ?, ?, ?, ?, ?)";
	char msg[500];
	char *desc[] = {
		"ФИО",
		"Страх. комп.",
		"Н. договора",
		"Дата рождения",
		"Пол",
		"Адрес",
		"Телефон"
	};
	char *regexes[] = {
		"^\\w",
		"^\\w",
		"^\\w",
		"^[0-9]{4}-[0-9]{2}-[0-9]{2}\\s+$",
		"^(М|Ж){1}\\s+$",
		"^\\w",
		"^[+][7][0-9]{10}\\s+$"
	};
	char **data = get_input("Заполните данные пациента", desc, 7, 1, 50, regexes);
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL); 
	sqlite3_bind_text(stmt, 1, data[0], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, data[1], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, data[2], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, data[3], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, data[4], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 6, data[5], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 7, data[6], -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		sql = "SELECT last_insert_rowid(), password FROM patient WHERE regid = last_insert_rowid()"; 
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL); 
		sqlite3_step(stmt);
		sprintf(msg, "\tЛогин P00%d\n\tПароль %s",
				sqlite3_column_int(stmt, 0),
				sqlite3_column_text(stmt, 1));
		message_box(msg, "Пациент создан!", -1, -1, 3, 40, 0);
	}
	else {
		for (int i = 0; i < 9; ++i) 
			if (sqlite3_step(stmt) == SQLITE_DONE) {
				mvprintw(1,1, "ВСё гуд");
				break;
			}
			else
				mvprintw(1,1, sqlite3_errmsg(db));
	}
	refresh();
	getch();
	for (int i = 0; i < 7; ++i)
		free(data[i]);
	free(data);
	sqlite3_finalize(stmt);
}

