#include <registry.h>

void registry_interface(void)
{
	char *options[] = {
		"Просмотр карт пациентов",
		"Добавить нового пациента",
		"Изменить данные пациента",
		"Удалить пациента",
		"Просмотр расписания врачей",
		"Создать запись к врачу",
		"Архив пациентов",
		"Выход",
		NULL
	};
	int regid = 0;
	while (TRUE) {
		switch (show_menu(options, 9, "Доступные действия", 4, -1)) {
			case 0:
				medcard_search();
				break;
			case 1:
				create_patient();
				break;
			case 2:
				alter_patient();
				break;
			case 3:
				delete_patient();
				break;
			case 4:
				timetable(0);
				break;
			case 5:
				regid = patient_search();
				appointment(regid);
				break;
			case 6:
				patient_archive();
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
	del_panel(pat->pan);
	delwin(pat->sub);
	delwin(pat->win);
	update_panels();
	doupdate();
}
int card_create(int regid)
{
	int z = 0;
	sqlite3_stmt *stmt;
	char *sql = "INSERT INTO medcard(regid, type) VALUES(?, ?)";
	char *type[] = {
		"Амбулаторная",
		"Стационарная",
		"Стамотологическая", 
		"Специальная",
		NULL
	};
	char msg[500];
	int choice = show_menu(type, 5, "Выберите тип карты", -1, -1);
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_text(stmt, 2, type[choice], -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 1, regid);
	if (z = (sqlite3_step(stmt) == SQLITE_DONE)) {
		sqlite3_finalize(stmt);
		sql = "SELECT last_insert_rowid()"; 
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL); 
		sqlite3_step(stmt);
		sprintf(msg, "Карта № %d успешно создана", sqlite3_column_int(stmt, 0));
		message_box(msg, "Создание карты", -1, -1, 3, 40, 0);
	}
	else if (z == SQLITE_CONSTRAINT)
		message_box("У пациента уже есть такая карта", "Ошибка", -1, -1, 3, 40, 0);
	

	sqlite3_finalize(stmt);
	return 0;
}

int create_patient(void)
{
	time_t now;
	sqlite3_stmt *stmt;
	char *sql = "INSERT INTO patient(fio, insurcomp, icontract, birthdate, gender, address, pnumber) VALUES(?, ?, ?, ?, ?, ?, ?)";
	char msg[500], today[30], **data = NULL;
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
		"^[1-2]{1}[0-9]{3}-[0-1]{1}[0-9]{1}-[0-3]{1}[0-9]{1}\\s+$",
		"^(М|Ж){1}\\s+$",
		"^\\w",
		"^[+][7][0-9]{10}\\s+$"
	};
INPUT:
	data = get_input("Заполните данные пациента", desc, 7, 1, 50, regexes, NULL, NULL);
	now = time(NULL);
	strftime(today, 29, "%Y-%m-%d", localtime(&now));
	if (strcmp(today, data[3]) < 0 || strcmp(data[3], "1900-01-01") < 0) {
		message_box("Введена некорректная дата", "Ошибка!", -1, -1, 3, 30, 0);
		goto INPUT;
	}
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

int alter_patient(void)
{
	char *sql = "SELECT regid, fio FROM patient WHERE fio LIKE ?";
	char mask[500], *s;
	time_t now;
	char today[30];
	char **data;
	sqlite3_stmt *stmt;
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
	sqlite3_finalize(stmt);

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
		"^[1-2]{1}[0-9]{3}-[0-1]{1}[0-9]{1}-[0-3]{1}[0-9]{1}\\s+$",
		"^(М|Ж){1}\\s+$",
		"^\\w",
		"^[+][7][0-9]{10}\\s+$"
	};
	sql = "SELECT fio, insurcomp, icontract, birthdate, gender, address, pnumber FROM patient where regid = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL); 
	sqlite3_bind_int(stmt, 1, regid);
	sqlite3_step(stmt);
	char **defaults = malloc(sizeof(char *) * 7);
	for (int i = 0; i < 7; ++i) {
		defaults[i] = malloc(strlen(sqlite3_column_text(stmt, i)) + 3);
		strcpy(defaults[i], sqlite3_column_text(stmt, i));
	}
	sqlite3_finalize(stmt);
INPUT:
	data = get_input("Изменение данных пациента", desc, 7, 1, 50, regexes, defaults, NULL);
	now = time(NULL);
	strftime(today, 29, "%Y-%m-%d", localtime(&now));
	if (strcmp(today, data[3]) < 0 || strcmp(data[3], "1900-01-01") < 0) {
		message_box("Введена некорректная дата", "Ошибка!", -1, -1, 3, 30, 0);
		goto INPUT;
	}

	sql = "UPDATE patient set fio = ?, insurcomp = ?, icontract = ?, birthdate = ?, gender = ?, address = ?, pnumber = ? where regid = ?";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL); 
	sqlite3_bind_text(stmt, 1, data[0], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, data[1], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, data[2], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 4, data[3], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 5, data[4], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 6, data[5], -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 7, data[6], -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 8, regid);
	if (sqlite3_step(stmt) == SQLITE_DONE) {
		sqlite3_finalize(stmt);
		strcpy(msg, "Данные успешно обновлены!");
		message_box(msg, "Обновление данных", -1, -1, 3, 40, 0);
	}
	else {
		mvprintw(1,1, sqlite3_errmsg(db));
	}
	refresh();
	for (int i = 0; i < 7; ++i)
		free(data[i]);
	free(data);
	for (int i = 0; i < 7; ++i) {
		free(defaults[i]);
	}
	free(defaults);
}

int delete_patient(void)
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
	sqlite3_finalize(stmt);
	sql = "DELETE FROM patient WHERE regid = ?";
	choice = message_box("Вы уверены?", "ВНИМАНИЕ!", -1, -1, 3, 25, 1);
	if (choice) {
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_bind_int(stmt, 1, regid);
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		message_box("Пациент перемещён в архив", "ОК", -1, -1, 3, 35, 0);
	}
	return 0;
}

int patient_archive(void)
{
	int choice;
	sqlite3_stmt *stmt;
	char *s, mask[500];
	char *sql = "SELECT regid, fio FROM patient_history WHERE fio LIKE ?";
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
	struct win_pan *pat = patient_info_hist(regid);
	medical_cards_history(regid);

	del_panel(pat->pan);
	delwin(pat->sub);
	delwin(pat->win);
	update_panels();
	doupdate();
}

int medical_cards_history(int regid) //work with patient's medical cards
{
	int i = 0, j = 0;
	int cards = get_card_amount_hist(regid);
	j = cards - 1;	

	PANEL **pmedcards = malloc(sizeof(PANEL *) * cards);
	WINDOW **wmedcards = malloc(sizeof(WINDOW *) * cards);
	WINDOW **subs = malloc(sizeof(WINDOW *) * cards);

	bind_windows(pmedcards, wmedcards, subs, cards);

	card_populate_hist(pmedcards, wmedcards, subs, regid);

	update_panels();
	doupdate();
	while ((i = getch()) != 'q') {
		switch(i) {
			case KEY_LEFT:
			case 'h':
				if (j > 0) 
					top_panel(pmedcards[--j]);
				break;
			case 'l':
			case KEY_RIGHT:
				if (j < cards - 1)
					top_panel(pmedcards[++j]);
				break;
			case 10:
				show_card_hist(*((int *)panel_userptr(pmedcards[j])));
		}
		update_panels();
		doupdate();
	}
  			
EXIT:
	i = 0;
	while (i < cards) {
		free((int *)panel_userptr(pmedcards[i]));
		del_panel(pmedcards[i]);
		wclear(wmedcards[i]);
		wrefresh(wmedcards[i]);
		delwin(subs[i]);
		delwin(wmedcards[i++]);
	}
	update_panels();
	doupdate();

	return j;
}

void card_populate_hist(PANEL **pmedcards, WINDOW **wmedcards, WINDOW **subs, int regid)
{
	int i = 0, *userptr;
	const char *sql = "SELECT * FROM patient_medcard_history where regid = ?";

	sqlite3_stmt *stmt;

	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);

	while(sqlite3_step(stmt) != SQLITE_DONE) {
		mvwprintw(wmedcards[i], 1, 2, "Карта № MC3070%d", sqlite3_column_int(stmt, 1));
		mvwaddch(wmedcards[i], 2, 0, ACS_LTEE);
		mvwhline(wmedcards[i], 2, 1, ACS_HLINE, 48);
		mvwaddch(wmedcards[i], 2, 49, ACS_RTEE);
		mvwprintw(subs[i], 4, 2, "ФИО: %s", sqlite3_column_text(stmt, 2)); 
		mvwprintw(subs[i], 5, 2, "Дата заведения: %s", sqlite3_column_text(stmt, 3)); 
		mvwprintw(subs[i], 6, 2, "Тип медкарты: %s", sqlite3_column_text(stmt, 4)); 
		mvwprintw(subs[i], 7, 2, "Количество записей: %s", sqlite3_column_text(stmt, 5)); 

		userptr = malloc(sizeof(int));
		*userptr = sqlite3_column_int(stmt, 1);

		set_panel_userptr(pmedcards[i], userptr);
		++i;
	}
	sqlite3_finalize(stmt);
}

void show_card_hist(int cardid)
{
	int t = 0, i = 0, j = 0, c;
	int rc; //record count
    const char *temp;
	sqlite3_stmt *stmt;

	sqlite3_prepare_v2(db, REQORD_AMOUNT_REQUEST_HIST, strlen(REQORD_AMOUNT_REQUEST_HIST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, cardid);
	sqlite3_step(stmt);
	rc = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);
	if (!rc)
		return;

	WINDOW *wmedc, *sub;
	PANEL *pmedc;

	wmedc = newwin(15, 80, LINES / 2 - 1, (COLS - 70) / 2);
	sub = derwin(wmedc, 13, 72, 1, 3);
	box(wmedc, 0, 0);
	keypad(wmedc, TRUE);
	pmedc = new_panel(wmedc);
	update_panels();
	doupdate();

	sqlite3_prepare_v2(db, MEDCARD_REQUEST_HIST, strlen(MEDCARD_REQUEST_HIST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, cardid);
	sqlite3_step(stmt);
	int row = 0, y, x;

	do {
DRAW:			werase(sub);	
				row = 1;
				temp = sqlite3_column_text(stmt, 0);
				t = temp[0];
				mvwprintw(sub, row++, 0, "Тип записи: %s", 
						t == 'V' ? "Визит": (t == 'T' ? "Лечение": "Анализы"));
				mvwprintw(sub, row++, 0, "Таб. номер врача: %s", sqlite3_column_text(stmt, 1));
				mvwprintw(sub, row++, 0, "ФИО отвественного врача: %s", sqlite3_column_text(stmt, 2));
				mvwprintw(sub, row++, 0, "Должность врача: %s", sqlite3_column_text(stmt, 3));
				mvwprintw(sub, row++, 0, "Дата внесения записи: %s", sqlite3_column_text(stmt, 5));
				mvwprintw(sub, row++, 0, "%s: %s", 
						t == 'V' ? "Цель визита": (t == 'T' ? "Заболевание": "Вид анализов"), 
						sqlite3_column_text(stmt, 6));
				getyx(sub, y, x);
				mvwprintw(sub, y + 1, 0, "%s %s", 
						t == 'V' ? "": (t == 'T' ? "Назначенное лечение:": "Результаты анализов:"), 
						sqlite3_column_text(stmt, 7));
				wmove(sub, 11, 33); wclrtoeol(sub);
				mvwprintw(sub, 12, 33, "%d/%d", i + 1, rc);
				wrefresh(sub);
				while ((t = wgetch(wmedc)) != 'q') 
					switch (t) {
						case 'l':
						case KEY_RIGHT:
							if (sqlite3_step(stmt) == SQLITE_DONE) {
								sqlite3_reset(stmt);
								for (int k = 0; k <= i; ++k)
									sqlite3_step(stmt);
							}
							else {
								++i;
								goto DRAW;

							}
							break;
						case 'h':
						case KEY_LEFT:
							if (0 == i)
								break;
							sqlite3_reset(stmt);
							for (j = 0; j < i; ++j) {
								sqlite3_step(stmt);
							}
							i = j - 1;
							goto DRAW; 
					}
				break;
	} while(TRUE); 
	sqlite3_finalize(stmt);
	del_panel(pmedc);
	delwin(sub);
	delwin(wmedc);
	update_panels();
	doupdate();
	return;
}



int get_card_amount_hist(int regid)
{
	int amount = 7;	
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, CARD_AMOUNT_REQUEST_HIST, strlen(CARD_AMOUNT_REQUEST_HIST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);
	sqlite3_step(stmt);
	amount = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);
	return amount;
}	

struct win_pan *patient_info_hist(int regid)
{
	sqlite3_stmt *stmt;
	WINDOW *patient_win, *patient_sub;

	PANEL *patient_pan;
	patient_win = newwin(25, 70, 4, 4);
	patient_sub = derwin(patient_win, 23, 60, 1, 4);
	patient_pan = new_panel(patient_win);
	sqlite3_prepare_v2(db, PATIENT_INFO_REQUEST_HIST, strlen(PATIENT_INFO_REQUEST_HIST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);
	sqlite3_step(stmt);

	int rows, i;
	rows = i = 0;
	for (int j = sqlite3_column_count(stmt) - 1; i < j; ++i) {
		if (strlen(sqlite3_column_text(stmt, i)) > 55)
			mvwprintw(patient_sub, 2 + rows++, 0, "%s: %s",
					sqlite3_column_name(stmt, i), sqlite3_column_text(stmt, i));
		else
			mvwprintw(patient_sub, 2 + rows, 0, "%s: %s",
					sqlite3_column_name(stmt, i), sqlite3_column_text(stmt, i));
		++rows;
	}
	sqlite3_finalize(stmt);	
	wrefresh(patient_sub);
				
	box(patient_win, 0,0);
	struct win_pan *resp = malloc(sizeof(struct win_pan));
	resp->win = patient_win;
	resp->pan = patient_pan;
	resp->sub = patient_sub;
	return resp;
}
