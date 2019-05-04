#include <patient.h>

int inside (char (*array)[50], char *value, int size);

void patient_interface(int regid) // the boss function to manage the view of interface
{
	sqlite3_stmt *stmt;
	char temp[1000];

	int i = 0, rows, cols;

	WINDOW *win_main_menu;

	PANEL *menu;
	MENU *main_menu;
	char *choices[] = {
						"Запись к врачу",
						"Расписание врачей",
						"Медицинские книжки",
						"Смена пароля",
						"Выход"
					  };

	ITEM **menu_items; 

	init_menu(&menu_items, choices, sizeof(choices) / sizeof(char *));
	main_menu = new_menu((ITEM **)menu_items);
	scale_menu(main_menu, &rows, &cols);

	win_main_menu = newwin(rows + 4, cols + 4, 4, 75); 
	set_menu_win(main_menu, win_main_menu); 
	set_menu_sub(main_menu, derwin(win_main_menu, rows, cols, 2, 2)); 
	menu = new_panel(win_main_menu);
	struct win_pan *pat = patient_info(regid);


	keypad(win_main_menu, TRUE); 
	box(win_main_menu, 0, 0);

	post_menu(main_menu);
	update_panels();

	doupdate();
	while((i = wgetch(win_main_menu)) != KEY_F(2))
	{   switch(i)
	    {	case KEY_DOWN:
		        menu_driver(main_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(main_menu, REQ_UP_ITEM);
				break;
			case 10:
				if (current_item(main_menu) == menu_items[0]) //EXAMPLE OF TERRIBLE CODING
					appointment(regid);
				else if (current_item(main_menu) == menu_items[1])
					timetable(0);
				else if (current_item(main_menu) == menu_items[2])
					medical_cards(regid, 0);
				else if (current_item(main_menu) == menu_items[3])
					password_change(regid);
				else
					goto EXIT;
		}
	}	

EXIT:
	i = 0;
	while(*(menu_items + i)) { //cleaning up
		free_item(*(menu_items + i++));
	}
	i = 0;
	del_panel(menu);
	del_panel(pat->pan);
	delwin(win_main_menu);
	delwin(pat->sub);
	delwin(pat->win);
	free(menu_items);
	free_menu(main_menu);
}

void init_menu(ITEM ***some_items, char **choices, size_t n_choices) // general function for menu initializing
{
	int i = -1;
	*some_items = (ITEM**)calloc(n_choices + 1, sizeof(ITEM *));
 	while(++i < n_choices) 
		(*some_items)[i] = new_item(choices[i], NULL);
	(*some_items)[n_choices] = NULL;
}

void appointment(int regid) //function for appointment creation
{
	int tabid = timetable(1);
	int i = 0, dos = 0;
	time_t now = time(NULL);	
	char *days[8], tarray[100];
	const int sec_in_day = 86400;
	char *sql = "SELECT weekday FROM timetable WHERE tabid = ? AND shiftst IS NULL";
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);
	char dayoffs[7][50];
	for (dos = 0; sqlite3_step(stmt) != SQLITE_DONE; ++dos) {
		strcpy(dayoffs[dos], sqlite3_column_text(stmt, 0));
		strip(dayoffs[dos]);
	}

	now += sec_in_day;
	for (int i = 0; i < 7; ++i) {
		days[i] = calloc(200, 1);	
		strftime(tarray, 98, "%A", localtime(&now));
		if (dos && inside(dayoffs, tarray, dos)) 
			strcpy(days[i], "Выходной день");
		else 
			strftime(days[i], 199, "%d.%m.%Y %A", localtime(&now));
		now += sec_in_day;
	}
	sqlite3_finalize(stmt);
	days[7] = NULL;
	int choice;
	while (TRUE) { 
		choice = show_menu(days, 8, "Выберите дату записи", 10, COLS - 80);
		if (strcmp(days[choice], "Выходной день"))
			break;
	}

	sql = "SELECT vacant_time(?, ?)";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);
	char temp[500] = {0};
	char backup[500] = {0};
	now = time(NULL);	
	now += sec_in_day * (choice + 1);
	strftime(temp, 99, "%Y-%m-%d", localtime(&now));
	sqlite3_bind_text(stmt, 2, temp, -1, SQLITE_STATIC);
	sqlite3_step(stmt);
	char **vacant_times = NULL;
	for (i = 0; ((char**)sqlite3_column_blob(stmt, 0))[i]; ++i) {
		vacant_times = realloc(vacant_times, sizeof(char *) * (i + 2));
		vacant_times[i] = malloc(50);
		vacant_times[i + 1] = NULL;
		sprintf(vacant_times[i], " %s", ((char**)sqlite3_column_blob(stmt, 0))[i]);
	}
	strcpy(backup, temp);
	sqlite3_finalize(stmt);
	choice = show_menu(vacant_times, i + 1, "ВРЕМЯ ЗАПИСИ", -1, -1);
	sql = "INSERT INTO appointment values(?, ?, ?)";
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);
	sqlite3_bind_int(stmt, 2, regid);
	sqlite3_bind_text(stmt, 3, strcat(temp, vacant_times[choice]), -1, SQLITE_STATIC);
	if (sqlite3_step(stmt) == SQLITE_CONSTRAINT) {
		sqlite3_finalize(stmt);
		sql = "SELECT e.fio, a.recdatetime FROM appointment a \
			   INNER JOIN employee e ON a.tabid = e.tabid where a.regid = ? and a.tabid = ?";
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_bind_int(stmt, 1, regid);
		sqlite3_bind_int(stmt, 2, tabid);
		sqlite3_step(stmt); 
		sprintf(temp, "\tУ вас уже есть запись к врачу\n\t%s\n\tна %s\n\tЗаменить запись?",
				sqlite3_column_text(stmt, 0),
				sqlite3_column_text(stmt, 1));
		if (message_box(temp, "Произошла ошибка", -1, -1, 8, 40, 1)) {
			sqlite3_finalize(stmt);
			sql = "UPDATE appointment SET recdatetime = ? where regid = ? and tabid = ?";
			sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
			sqlite3_bind_int(stmt, 3, tabid);
			sqlite3_bind_int(stmt, 2, regid);
			sqlite3_bind_text(stmt, 1, strcat(backup, vacant_times[choice]), -1, SQLITE_STATIC);
			sqlite3_step(stmt); 
			message_box("Запись успешно обновлена", "Обновление записи", -1, -1, 3, 40, 0);
		}
	}
	else {
		sqlite3_finalize(stmt);
		sql = "SELECT e.fio, a.recdatetime FROM appointment a \
			   INNER JOIN employee e ON a.tabid = e.tabid where a.regid = ? and a.tabid = ?";
		sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
		sqlite3_bind_int(stmt, 1, regid);
		sqlite3_bind_int(stmt, 2, tabid);
		sqlite3_step(stmt); 
		sprintf(temp, "\tЗапись к врачу\n\t%s\n\tна %s создана.",
				sqlite3_column_text(stmt, 0),
				sqlite3_column_text(stmt, 1));
		message_box(temp, "Запись создана", -1, -1, 4, 40, 0);
	}
	sqlite3_finalize(stmt);
	for (int i = 0; vacant_times[i]; ++i) 
		free(vacant_times[i]);
	free(vacant_times);
}

int timetable(int mode) //doctor's timetable view
{
	int i = 0;
	WINDOW *dwindow;
	PANEL *dpanel;
	DMS cleanup = NULL;
	MENU *dmenu = doctor_list(&cleanup);
	dwindow = newwin(23, 49, 10, 60);
	dpanel = new_panel(dwindow);
	box(dwindow, 0, 0);
	
	mvwaddch(dwindow, 2, 0, ACS_LTEE);
	mvwhline(dwindow, 2, 1, ACS_HLINE, 47);
	mvwaddch(dwindow, 2, 48, ACS_RTEE);

	wattron(dwindow, COLOR_PAIR(GREEN));
	mvwprintw(dwindow, 1, 18, "СПИСОК ВРАЧЕЙ");
	wattroff(dwindow, COLOR_PAIR(GREEN));

	set_menu_win(dmenu, dwindow); 
	set_menu_sub(dmenu, derwin(dwindow, 17, 45, 4, 2)); 
	post_menu(dmenu);
	keypad(dwindow, TRUE); 
	update_panels();
	doupdate();
	int retval = 0;
	
	while((i = wgetch(dwindow)) != KEY_F(2))
	{   switch(i)
	    {	case KEY_DOWN:
		        menu_driver(dmenu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(dmenu, REQ_UP_ITEM);
				break;
			case 10:
				if (mode == 0)
					print_timetable(*(int *)(item_userptr(current_item(dmenu))));
				else if (mode == 1) {
					retval = *(int *)(item_userptr(current_item(dmenu)));
					goto EXIT;
				}
				break;
		}
	}	
EXIT:
	free_ms(cleanup);
	free_menu(dmenu);
	del_panel(dpanel);
	werase(dwindow);
	delwin(dwindow);
	update_panels();
	doupdate();
	return retval;
}


void print_timetable(int tabid)
{
	char *sql = "SELECT weekday, get_time(shiftst), get_time(shiftend), get_time(break) FROM timetable where tabid = ? and shiftst IS NOT NULL ORDER BY daynum";
	char *days[8] = {NULL};
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, tabid);

	int i = 0;
	for (i = 0; sqlite3_step(stmt) != SQLITE_DONE; ++i) {
		days[i] = calloc(200, 1);
		sprintf(days[i], "%s %8s%8s%8s",
				sqlite3_column_text(stmt, 0),
				sqlite3_column_text(stmt, 1),
				sqlite3_column_text(stmt, 2),
				sqlite3_column_text(stmt, 3));
	}
	char *message = "\tДень\t\tНачало\t Конец \tПерерыв";
	show_menu(days, i + 1, message, -1, -1);
	for (int i = 0; i < 7; ++i)
		free(days[i]);
	sqlite3_finalize(stmt);
	return;
}


MENU *doctor_list(DMS (*clup)) //argument is a cleanup structure
{
	(*clup) = malloc(sizeof(struct menu_struct));
	(*clup)->dlist = (*clup)->plist = NULL;
	(*clup)->doctors = NULL;
	int dlcount = 0, textbytes, *tabidptr;
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, DOCTOR_LIST_REQUEST, strlen(DOCTOR_LIST_REQUEST), &stmt, NULL);
	MENU *dmenu;

 	while (sqlite3_step(stmt) != SQLITE_DONE) {
		(*clup)->dlist = realloc((*clup)->dlist, ++dlcount * sizeof(char *));
		(*clup)->plist = realloc((*clup)->plist, dlcount * sizeof(char *));
		textbytes = strlen(sqlite3_column_text(stmt, 0)) + 1;
		((*clup)->dlist)[dlcount - 1] = malloc(textbytes);
		strncpy(((*clup)->dlist)[dlcount - 1], sqlite3_column_text(stmt, 0), textbytes);
		textbytes = strlen(sqlite3_column_text(stmt, 1)) + 1;
		((*clup)->plist)[dlcount - 1] = malloc(textbytes);
		strncpy(((*clup)->plist)[dlcount - 1], sqlite3_column_text(stmt, 1), textbytes);
		(*clup)->doctors = realloc((*clup)->doctors, dlcount * sizeof(ITEM *));
		((*clup)->doctors)[dlcount - 1] = new_item(((*clup)->dlist)[dlcount - 1], (*clup)->plist[dlcount - 1]); //menu creation
		
		tabidptr = malloc(sizeof(int));
		*tabidptr = sqlite3_column_int(stmt, 2);
		set_item_userptr(((*clup)->doctors)[dlcount - 1], tabidptr); 
		}

		(*clup)->dlist = realloc((*clup)->dlist, (dlcount + 1) * sizeof(char *)); //in order to mark the end of list for subsequent cleanup procedure
		(*clup)->plist = realloc((*clup)->plist, (dlcount + 1) * sizeof(char *));
		(*clup)->doctors = realloc((*clup)->doctors, (dlcount + 1) * sizeof(ITEM *));
		((*clup)->dlist)[dlcount] = NULL;
		((*clup)->plist)[dlcount] = NULL;
		((*clup)->doctors)[dlcount] = NULL;
		dmenu = new_menu((*clup)->doctors);
		return dmenu;
}

void password_change(int regid) //function for appointment creation
{
	int i = 0;
	bool SUCCESS = FALSE;
	WINDOW *wpass_change; 			
	PANEL *ppass_change;		
	FORM *fpass_change;
	FIELD **fpc = malloc(3 * sizeof(FIELD *));
	fpc[0] = new_field(1, 27, 3, 18, 0, 0);
	fpc[1] = new_field(1, 27, 7, 18, 0, 0);
	fpc[2] = NULL;
	fpass_change = new_form(fpc);
	set_field_opts(fpc[0], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	set_field_opts(fpc[1], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	form_opts_off(fpass_change, O_BS_OVERLOAD); //neccessary in order to prevent strange behaviour of backspace key

	wpass_change = newwin(13, 50, 10, 50);

	set_form_win(fpass_change, wpass_change);
	set_form_sub(fpass_change, derwin(wpass_change, 10, 50, 3, 0));

	post_form(fpass_change);	
	box(wpass_change, 0, 0);

	ppass_change = new_panel(wpass_change);
	update_panels();
	mvwaddch(wpass_change, 2, 0, ACS_LTEE);
	mvwhline(wpass_change, 2, 1, ACS_HLINE, 49);
	mvwaddch(wpass_change, 2, 49, ACS_RTEE);

	wattron(wpass_change, COLOR_PAIR(RED));
	mvwprintw(wpass_change, 1, 13, "Введите новый пароль ниже");
	wattroff(wpass_change, COLOR_PAIR(RED));

	mvwaddch(wpass_change, 5, 17, ACS_ULCORNER);
	mvwaddch(wpass_change, 7, 17, ACS_LLCORNER);
	mvwhline(wpass_change, 5, 18, ACS_HLINE, 29);
	mvwhline(wpass_change, 7, 18, ACS_HLINE, 29);
	mvwaddch(wpass_change, 5, 46, ACS_URCORNER);
	mvwaddch(wpass_change, 7, 46, ACS_LRCORNER);
	mvwvline(wpass_change, 6, 17, ACS_VLINE, 1);
	mvwvline(wpass_change, 6, 46, ACS_VLINE, 1);

	mvwaddch(wpass_change, 9, 17, ACS_ULCORNER);
	mvwaddch(wpass_change, 11, 17, ACS_LLCORNER);
	mvwhline(wpass_change, 9, 18, ACS_HLINE, 29);
	mvwhline(wpass_change, 11, 18, ACS_HLINE, 29);
	mvwaddch(wpass_change, 9, 46, ACS_URCORNER);
	mvwaddch(wpass_change, 11, 46, ACS_LRCORNER);
	mvwvline(wpass_change, 10, 17, ACS_VLINE, 1);
	mvwvline(wpass_change, 10, 46, ACS_VLINE, 1);
	wrefresh(wpass_change);
	doupdate();

	wattron(wpass_change, COLOR_PAIR(GREEN));
	mvwprintw(wpass_change, 6, 3, "Новый пароль");
	mvwprintw(wpass_change, 10, 2, "Подтверждение");
	wattroff(wpass_change, COLOR_PAIR(GREEN));
	
    keypad(wpass_change, TRUE); //let every keystroke through

	while (!SUCCESS && (i = wgetch(wpass_change))) //control loop to manage form 
		switch(i) {
			case KEY_UP:
				form_driver(fpass_change, REQ_PREV_FIELD);
				form_driver(fpass_change, REQ_END_LINE);
				break;
			case '\t':
			case KEY_DOWN:
				form_driver(fpass_change, REQ_NEXT_FIELD);
				form_driver(fpass_change, REQ_END_LINE);
				break;
			case KEY_LEFT:
				form_driver(fpass_change, REQ_LEFT_CHAR);
				break;
			case KEY_RIGHT:
				form_driver(fpass_change, REQ_RIGHT_CHAR);
				break;
			case KEY_DC:
			case KEY_BACKSPACE:
			case 127:
				form_driver(fpass_change, REQ_DEL_PREV);
				break;
			case 10:
				if (!strcmp(field_buffer(fpc[0], 0), field_buffer(fpc[1], 0))) {
					update_pass(regid, field_buffer(fpc[0], 0));
					move(LINES - 2, 3); clrtoeol();	
					SUCCESS = TRUE;

					del_panel(ppass_change);
					unpost_form(fpass_change);
					free_form(fpass_change);
					free_field(fpc[0]); free_field(fpc[1]);
					free(fpc);
					werase(wpass_change);
					wrefresh(wpass_change);
					delwin(wpass_change);
					update_panels();
					doupdate();
					mvprintw(LINES - 2, 4, "Пароль успешно обновлен");
					}
				else {
					mvprintw(LINES - 2, 4, "Пароли не совпадают");
				}
				refresh();
				break;
			default:
				form_driver(fpass_change, i); 
				form_driver(fpass_change, REQ_END_LINE);
				break;
		}
	getch();
	move(LINES - 2, 3); clrtoeol();	
	return;	
}

void update_pass(int regid, char *new_pass)
{
	char temporary[100], i;
	for (i = -1; *(new_pass + ++i) != ' ' && i < 100; temporary[i] = new_pass[i]);
	temporary[i] = '\0';
	char *sql = PATIENT_PASS_UPDATE;
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 2, regid);
	sqlite3_bind_text(stmt, 1, temporary, strlen(temporary), SQLITE_STATIC);
	sqlite3_step(stmt);
	sqlite3_finalize(stmt);
}



void free_ms(DMS clup)
{
	for (int i = 0; *(clup->doctors + i) != NULL; i++) {
		free(item_userptr(*(clup->doctors + i)));
		free_item(*(clup->doctors + i));
	}
	for (int i = 0; *(clup->dlist + i) != NULL; i++) {
		free(*(clup->dlist + i)); 
		free(*(clup->plist +i));
	}
}

int inside (char (*array)[50], char *value, int size)
{
	for (int i = 0; i < size; ++i)
		if (!strcmp(array[i], value))
			return 1;
	return 0;
}
