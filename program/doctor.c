#include <doctor.h>

void doctor_interface(int tabid)
{
	int regid, repeat = 0;
SEARCH:
	regid = patient_search();
	struct win_pan *pat = patient_info(regid);
	char *options[] = {
		"Просмотр карт пациента",
		"Назначение анализов",
		"Внести результат анализов",
		"Назначение лечения",
		"Выписка рецепта",
		"Выдача листка нетрудоспособности",
		"Просмотр листов нетрудоспособности",
		"Повторить поиск",
		"Выход",
		NULL
	};
	while(TRUE) {
		switch (show_menu(options, 10, "Выберите действие", 4, COLS - 70)) {
			case 0:
				medical_cards(regid, 0);
				break;
			case 1:
				annalysis_create(tabid, regid);
				break;
			case 2:
				annalysis_update(regid);
				break;
			case 3:
				cure_create(regid, tabid);
				break;
			case 4:
				receipt_issue(tabid, regid);
				break;
			case 5:
				sick_leave_issue(regid, tabid);
				break;
			case 6:
				sickleave_list(tabid, regid);
				break;
			case 7:
				repeat = 1;
				goto EXIT;
				break;
			default:
				repeat = 0;
				goto EXIT;
		}
	}

EXIT:
	del_panel(pat->pan);
	delwin(pat->sub);
	delwin(pat->win);
	update_panels();
	doupdate();
	if (repeat) {
		repeat = 0;
		goto SEARCH;
	}
	return;
}

int annalysis_create(int tabid, int regid)
{
	sqlite3_stmt *stmt;
	message_box("Выберите карту пациента", NULL, 5, COLS - 50, 2, 25, 0);	
	int cardid = medical_cards(regid, 1);
	char *descriptions[] = {
		"Вид анализов",
	};

	char msg[100];
	char *sql; 

	char **fields = get_input("Назначение анализов", descriptions, 1, 5, 35, NULL, NULL, NULL);
	sql = "INSERT INTO analysis(tabid, cardid, type) values(?, ?, ?)";
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
		message_box(msg, "Номер анализа", -1, -1, 4, 45, 0);
	}
	else 
		mvprintw(1, 1, sqlite3_errmsg(db));

	sqlite3_finalize(stmt);
	refresh();
	free(fields[0]);
	free(fields);
	return 0;
}

int annalysis_update(int regid)
{
	sqlite3_stmt *stmt;
	char *descriptions[] = {
		"Номер анализов",
		"Результат",
	};
	char *sql = "SELECT anid, type FROM analysis WHERE cardid IN (SELECT cardid FROM\
				 medcard WHERE regid = ?) AND result IS NULL";
	char **ansis_set = NULL;
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);
	int z = 0;
	while(sqlite3_step(stmt) != SQLITE_DONE) {
		ansis_set = realloc(ansis_set, sizeof(char *) * (z + 2));
		ansis_set[z] = malloc(strlen(sqlite3_column_text(stmt, 1)) + 11);
		sprintf(ansis_set[z], "%04d %-s", 
				sqlite3_column_int(stmt, 0),
				sqlite3_column_text(stmt, 1));
		++z;
	}
	ansis_set[z] = NULL;
	z = show_menu(ansis_set, z + 1, "Выберите анализы", -1, -1);
	sqlite3_reset(stmt);	
	for (int i = 0; i <= z; ++i) 
		sqlite3_step(stmt);
	int anid = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);

	char **defaults = malloc(sizeof(char *) * 2);
	defaults[0] = malloc(10);
	defaults[1] = malloc(5);
	sprintf(defaults[0], "%d", anid);
	sprintf(defaults[1], "\0");
		
		

	//TODO FINISH FUNCTIONS, modify get_input, to accept diff sizes, add default value to get_input as anid
	char **fields = get_input("Заполните поля", descriptions, 2, 4, 40, NULL, defaults, (int []){1, 4});
	sql = "UPDATE analysis SET result = ? where anid = ?";
	char msg[100];
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 2, anid);
	sqlite3_bind_text(stmt, 1, fields[1], -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sprintf(msg, "Анализ № %d успешно обновлен", anid);
		message_box(msg, "Обновление анализа", -1, -1, 4, 45, 0);
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
	message_box("Выберите карту пациента", NULL, 5, COLS - 50, 2, 25, 0);	
	int cardid = medical_cards(regid, 1);

	char *descriptions[] = {
		"Заболевание",
		"Лечение",
	};

	char **fields = get_input("Заполните поля", descriptions, 2, 5, 50, NULL, NULL, NULL);
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
		message_box(msg, "Создание Лечения", -1, -1, 4, 40, 0);
	}
	sqlite3_finalize(stmt);
	free(fields[0]);
	free(fields[1]);
	free(fields);

	return 0;
}
int receipt_issue(int tabid, int regid)
{
	sqlite3_stmt *stmt;

	char *descriptions[] = {
		"Лекарства",
	};

	char **fields = get_input("Заполните поля", descriptions, 1, 5, 50, NULL, NULL, NULL);
	char *sql = "INSERT INTO receipt(tabid, regid, medicine)\
				 values(?, ?, ?)";
	char msg[100];
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);
	sqlite3_bind_int(stmt, 2, regid);
	sqlite3_bind_text(stmt, 3, fields[0], -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		sql = "SELECT last_insert_rowid()"; 
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_step(stmt);
		sprintf(msg, "Рецепт № %d успешно добавлен", sqlite3_column_int(stmt, 0));
		message_box(msg, "Создание рецепта", -1, -1, 4, 40, 0);
	}
	sqlite3_finalize(stmt);
	free(fields[0]);
	free(fields);
	return 0;
}
int sick_leave_issue(int regid, int tabid)
{
	time_t now;
	char **fields;
	char *sql;
	char today[30];
	sqlite3_stmt *stmt;
	char *sick_codes[] = {
			"01 Заболевания",
			"02 Травма",
			"03 Карантин",
			"04 Несчастный случай на произ-ве",
			"05 По беременности",
			"06 Протезирование",
			"07 Профессиональное заболевание",
			"08 Долечивание",
			"09 Уход за больным членом семьи",
			NULL
		};
	int sick_code = show_menu(sick_codes, 10, "Причина нетрудоспособности", -1, -1) + 1; 

	char *descriptions[] = {
		"Дата начала",
		"Дата окончания",
		"Место назначения"
	};
	char *regexes[] = {
		"^[1-2]{1}[0-9]{3}-[0-1]{1}[0-9]{1}-[0-3]{1}[0-9]{1}\\s+$",
		"^[1-2]{1}[0-9]{3}-[0-1]{1}[0-9]{1}-[0-3]{1}[0-9]{1}\\s+$",
		"^\\w",
	};

INPUT:
	fields = get_input("Создание больничного", descriptions, 3, 1, 30, regexes, NULL, NULL);
	sql = "INSERT INTO sickleave(regid, tabid, stdate, endate, destn, sick_code)\
				 values(?, ?, ?, ?, ?, ?)";
	now = time(NULL);
	strftime(today, 29, "%Y-%m-%d", localtime(&now));
	if (strcmp(today, fields[0]) < 0 || strcmp(fields[0], "2000-01-01") < 0) {
		message_box("Введена некорректная дата начала", "Ошибка!", -1, -1, 3, 34, 0);
		goto INPUT;
	}
	if (strcmp(today, fields[1]) < 0 || strcmp(fields[1], "2000-01-01") < 0) {
		message_box("Введена некорректная дата окончания", "Ошибка!", -1, -1, 3, 37, 0);
		goto INPUT;
	}
	if (strcmp(fields[0], fields[1]) > 0) {
		message_box("Дата начала больше даты окончания", "Ошибка!", -1, -1, 3, 40, 0);
		goto INPUT;
	}
	char msg[100];
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);
	sqlite3_bind_int(stmt, 2, tabid);
	sqlite3_bind_text(stmt, 3, fields[0], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, fields[1], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, fields[2], -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 6, sick_code);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		sql = "SELECT last_insert_rowid()"; 
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_step(stmt);
		sprintf(msg, "Больничный № %d успешно создан", sqlite3_column_int(stmt, 0));
		message_box(msg, "Выписка листа нетрудоспособности", -1, -1, 4, 45, 0);
	}
	sqlite3_finalize(stmt);
	free(fields[0]);
	free(fields[1]);
	free(fields[2]);
	free(fields);
	return 0;
}

int patient_search(void)
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
		message_box("Пациентов не найдено", "УПС!!!", -1, -1, 3, 25, 0);
		goto SEARCH;
	}

	patients[i] = NULL;
	int r = show_menu(patients, i + 1, "Выберите пациента!", -1, -1);
	sqlite3_reset(stmt);
	for (int j = 0; (sqlite3_step(stmt) != SQLITE_DONE) && (j < r); ++j);
	int regid = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);
	return regid;
}

int sickleave_list(int tabid, int regid)
{
	sqlite3_stmt *stmt;
	char *sql = "SELECT s.sickid, strftime('%d-%m-%Y', s.issuedate),\
				 strftime('%d-%m-%Y', s.stdate), \
				 strftime('%d-%m-%Y', s.endate),\
				 strftime('%d-%m-%Y',date(s.endate, '+1 day')), \
				 s.destn, e.fio, e.position, p.fio, p.gender, \
				 strftime('%d-%m-%Y',p.birthdate), s.sick_code \
				 FROM sickleave s \
				 JOIN employee e ON s.tabid = e.tabid \
				 JOIN patient p ON p.regid = s.regid\
				 where s.tabid = ? and s.regid = ?";
	char **sickleaves = NULL;
	char temp[1000];
	int count = 0, choice = 0;
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);
	sqlite3_bind_int(stmt, 2, regid);
	int canary = 0;
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		++canary;
		sprintf(temp, "SLN%03d - %s - %02d",
				sqlite3_column_int(stmt, 0),
				sqlite3_column_text(stmt, 1),
				sqlite3_column_int(stmt, 11));
		sickleaves = realloc(sickleaves, (count + 1) * sizeof(char *));
		sickleaves[count] = malloc(strlen(temp) + 1);
		strcpy(sickleaves[count], temp);
		count += 1;
	}
	sqlite3_finalize(stmt);
	if (!canary)
		return 0;
	sickleaves = realloc(sickleaves, (count + 1) * sizeof(char *));
	sickleaves[count] = NULL;
MENU:
	choice = show_menu(sickleaves, count + 1, "Листы нетрудоспособности", -1, -1);
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);
	sqlite3_bind_int(stmt, 2, regid);
	for (int i = 0; i <= choice; ++i)
		sqlite3_step(stmt);
	choice = show_sickleave_form(stmt);
	if (choice == 'q')
		goto MENU;

	for (int i = 0; i < count; ++i)
		free(sickleaves[i]);
	free(sickleaves);

return 0;
}

int show_sickleave_form(sqlite3_stmt *stmt)
{
	int w = 80, h = 29;
	WINDOW *rwin, *rsub;
	PANEL *rpan;
	rwin = newwin(h + 2, w + 10, 3, 40);
	rsub = derwin(rwin, h, w, 1, 5);
	rpan = new_panel(rwin);
	char **fio = space_parse(sqlite3_column_text(stmt, 8));
	mvwprintw(rsub, 1, 29, "ЛИСТОК НЕТРУДОСПОСОБНОСТИ");
	mvwchgat(rsub, 1, 24, -1, A_BOLD, 0, NULL);
	mvwprintw(rsub, 3, 0, "Номер листка нетрудоспособности: SLN%03d", sqlite3_column_int(stmt, 0));
	mvwchgat(rsub, 3, 33, -1, A_BOLD, 0, NULL);
	mvwprintw(rsub, 4, 0, "Наименование медицинской организации");
	mvwprintw(rsub, 5, 0, "ГОРОДСКАЯ ПОЛИКЛИНИКА №72");
	mvwchgat(rsub, 5, 0, -1, A_BOLD, 0, NULL);
	mvwprintw(rsub, 6, 0, "Адрес медицинской организации");
	mvwprintw(rsub, 7, 0, "СУЩЕВСКИЙ ВАЛ 21");
	mvwchgat(rsub, 7, 0, -1, A_BOLD, 0, NULL);
	mvwprintw(rsub, 8, 0, "Дата выдачи: %s", sqlite3_column_text(stmt, 1));
	mvwchgat(rsub, 8, 13, 10, A_BOLD, 0, NULL);
	mvwprintw(rsub, 8, 25, "ОГРН: 1057746061262");
	mvwchgat(rsub, 8, 30, -1, A_BOLD, 0, NULL);
	mvwprintw(rsub, 9, 0, "Ф: %s", fio[0]);
	mvwchgat(rsub, 9, 2, -1, A_BOLD, 0, NULL);
	mvwprintw(rsub, 10, 0, "И: %s", fio[1]);
	mvwchgat(rsub, 10, 2, -1, A_BOLD, 0, NULL);
	mvwprintw(rsub, 11, 0, "О: %s", fio[2] == NULL ? "" : fio[2]);
	mvwchgat(rsub, 11, 2, -1, A_BOLD, 0, NULL);
	mvwprintw(rsub, 12, 0, "Дата рождения: %s", sqlite3_column_text(stmt, 10));
	mvwchgat(rsub, 12, 15, 10, A_BOLD, 0, NULL);
	mvwprintw(rsub, 12, 27, "Пол: %s", sqlite3_column_text(stmt, 9));
	mvwchgat(rsub, 12, 32, 1, A_BOLD, 0, NULL);
	mvwprintw(rsub, 12, 35, "Причина нетрудоспособности (код): %02d", sqlite3_column_int(stmt, 11));
	mvwchgat(rsub, 12, 69, 2, A_BOLD, 0, NULL);
	mvwprintw(rsub, 13, 0, "Место работы/образовательной организации:");
	mvwprintw(rsub, 14, 0, "%s", sqlite3_column_text(stmt, 5));
	mvwchgat(rsub, 14, 0, -1, A_BOLD, 0, NULL);
	//===================================================
	mvwhline(rsub, 15, 0, ACS_HLINE, w - 1);
	//===================================================
	mvwprintw(rsub, 17, 30, "ОСВОБОЖДЕНИЕ ОТ РАБОТЫ");
	for (int i = 0; fio[i]; ++i)
		free(fio[i]);
	free(fio);
	fio = space_parse(sqlite3_column_text(stmt, 6));
	fio[1][2] = 0;
	if (fio[2])
		fio[2][2] = 0;
	//===================================================
	mvwhline(rsub, 18, 0, ACS_HLINE, w - 1);
	//===================================================

	mvwprintw(rsub, 19, 0, "С какого числа | По какое число |  Должность врача  | Фамилия и инициалы врача");
	wattron(rsub, A_BOLD);
	mvwprintw(rsub, 20, 0, "  %s   |   %s   |", 
		sqlite3_column_text(stmt, 2),
		sqlite3_column_text(stmt, 3));
	mvwprintw(rsub, 20, 34, " %s", sqlite3_column_text(stmt, 7));
	mvwprintw(rsub, 20, 52, "| %s %s.%s",
		fio[0],
		fio[1],
		fio[2]);
	wattroff(rsub, A_BOLD);
	//===================================================
	mvwhline(rsub, 22, 0, ACS_HLINE, w - 1);
	//===================================================
	mvwprintw(rsub, 24, 0, "Приступить к работе с: %s", sqlite3_column_text(stmt, 4));
	mvwchgat(rsub, 24, 23, 10, A_BOLD, 0, NULL);
	mvwprintw(rsub, 26, 0, "Подпись и личная печать");
	mvwprintw(rsub, 27, 0, "лечащего врача");
	for (int i = 0; fio[i]; ++i)
		free(fio[i]);
	free(fio);

	sqlite3_finalize(stmt);

	box(rwin, 0,0);
	int retval = wgetch(rwin);

	del_panel(rpan);
	delwin(rsub);
	delwin(rwin);
	update_panels();
	doupdate();
	return retval;	
}

char **space_parse(const char *text) 
{
	char **words = malloc(sizeof(char *));
	char temp[300];
	int j = 0, k = 0;
	for (int i = 0; text[i]; ++i)
		if (text[i] != ' ')
			temp[j++] = text[i];
		else {
			temp[j] = 0;
			words[k] = malloc(strlen(temp) + 1);
			strcpy(words[k], temp);
			j = 0;
			words = realloc(words, (++k + 2) * sizeof(char *));
		}
	temp[j] = 0;
	words[k] = malloc(strlen(temp) + 1);
	strcpy(words[k], temp);
	words[k + 1] = NULL;
	return words;
}
