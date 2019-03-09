#include <patient.h>

void patient_interface(int regid) // the boss function to manage the view of interface
{
	sqlite3_stmt *stmt;
	char temp[1000];

	int i = 0, rows, cols;

	WINDOW *win_main_menu, *patient_info;

	PANEL *menu, *patient;
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
	patient_info = newwin(25, 70, 4, 4);
	patient = new_panel(patient_info);

	sqlite3_prepare_v2(db, PATIENT_INFO_REQUEST, strlen(PATIENT_INFO_REQUEST), &stmt, NULL);
	sqlite3_bind_int(stmt, 0, regid);
	sqlite3_step(stmt);

	i = 0;
	for (int j = sqlite3_column_count(stmt); i < j; ++i)
		mvwprintw(patient_info, 2 + i, 3, "%s: %s",
				sqlite3_column_name(stmt, i), sqlite3_column_text(stmt, i));
	sqlite3_finalize(stmt);	
				
	box(patient_info, 0,0);

	keypad(win_main_menu, TRUE); 
	box(win_main_menu, 0, 0);

	post_menu(main_menu);
	update_panels();

	doupdate();
	while((i = wgetch(win_main_menu)) != KEY_F(1))
	{   switch(i)
	    {	case KEY_DOWN:
		        menu_driver(main_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(main_menu, REQ_UP_ITEM);
				break;
			case 10:
				if (current_item(main_menu) == menu_items[0]) //EXAMPLE OF TERRIBLE CODING
					appointement(regid);
				else if (current_item(main_menu) == menu_items[1])
					timetable();
				else if (current_item(main_menu) == menu_items[2])
					medical_cards(regid);
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
	del_panel(patient);
	delwin(win_main_menu);
	delwin(patient_info);
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

void appointement(int regid) //function for appointment creation
{
	mvprintw(LINES - 1, 2, "SUCCESS");
	refresh();
}

void timetable(void) //doctor's timetable view
{
	int i = 0;
	WINDOW *dwindow;
	PANEL *dpanel;
	DMS cleanup = NULL;
	MENU *dmenu = doctor_list(&cleanup);
	dwindow = newwin(25, 49, 10, 60);
	dpanel = new_panel(dwindow);
	box(dwindow, 0, 0);
	
	set_menu_win(dmenu, dwindow); 
	set_menu_sub(dmenu, derwin(dwindow, 22, 45, 3, 1)); 
	post_menu(dmenu);
	keypad(dwindow, TRUE); 
	update_panels();
	doupdate();
	
	while((i = wgetch(dwindow)) != KEY_F(1))
	{   switch(i)
	    {	case KEY_DOWN:
		        menu_driver(dmenu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(dmenu, REQ_UP_ITEM);
				break;
			case 10:
				print_timetable(*(int *)(item_userptr(current_item(dmenu))));
				break;
		}
	}	
	free_ms(cleanup);
	free_menu(dmenu);
	del_panel(dpanel);
	werase(dwindow);
	delwin(dwindow);
	update_panels();
	doupdate();
	return;
}

void print_timetable(int tabid)
{

	return;
}

MENU *doctor_list(DMS (*clup)) //arguments are doctors list and positions list
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

void medical_cards(int regid) //work with patient's medical cards
{
	int i = 0, j = 0;
	int cards = get_card_amount(regid);
	j = cards - 1;	

	PANEL **pmedcards = malloc(sizeof(PANEL *) * cards);
	WINDOW **wmedcards = malloc(sizeof(WINDOW *) * cards);

	bind_windows(pmedcards, wmedcards, cards);

	card_populate(pmedcards, wmedcards, regid);

	update_panels();
	doupdate();

	while ((i = getch()) != KEY_F(1)) {
		switch (i) {
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
				show_card(*((int *)panel_userptr(pmedcards[j])));
		}
		update_panels();
		doupdate();
	}
  			
	i = 0;
	while (i < cards) {
		free((int *)panel_userptr(pmedcards[i]));
		del_panel(pmedcards[i]);
		wclear(wmedcards[i]);
		wrefresh(wmedcards[i]);
		delwin(wmedcards[i++]);
	}

	return;
}
void password_change(int regid) //function for appointment creation
{
	WINDOW *wpass_change, new_pass, confirm_pass; 			
	PANEL *ppass_change;		
	FORM *fpass_change;
	FIELD **fpc = malloc(3 * sizeof(FIELD *));
	fpc[0] = new_field(1, 30, 2, strlen("Новый пароль: "), 0, 0);
	fpc[1] = new_field(1, 30, 4, strlen("Подтверждение: "), 0, 0);
	fpc[2] = NULL;
	fpass_change = new_form(fpc);
	set_field_opts(fpc[0], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	set_field_opts(fpc[1], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	

	wpass_change = newwin(9, 50, 10, 50);
	box(wpass_change, 0, 0);
	ppass_change = new_panel(wpass_change);
	update_panels();
	mvwaddch(wpass_change, 2, 0, ACS_LTEE);
	mvwhline(wpass_change, 2, 1, ACS_HLINE, 49);
	mvwaddch(wpass_change, 2, 50, ACS_RTEE);

	mvwaddch(wpass_change, 5, 17, ACS_ULCORNER);
	mvwaddch(wpass_change, 7, 17, ACS_LLCORNER);
	mvwhline(wpass_change, 5, 18, ACS_HLINE, 30);
	mvwhline(wpass_change, 7, 18, ACS_HLINE, 30);
	mvwaddch(wpass_change, 5, 48, ACS_URCORNER);
	mvwaddch(wpass_change, 7, 48, ACS_LRCORNER);
	mvwvline(wpass_change, 6, 17, ACS_VLINE, 1);
	mvwvline(wpass_change, 6, 48, ACS_VLINE, 1);
	wrefresh(wpass_change);
	doupdate();

	//new_pass = derwin(wpass_change, 3, 30, 3, 15);
	//confirm_pass = derwin(wpass_change, 3, 30, 7, 15);
	//box(new_pass, 0, 0);
	//box(confirm_pass, 0, 0);



	return;	
}

void show_card(int cardid)
{
	int t = 0, i = 0, j = 0;
	int rc; //record count
    const char *temp;
	WINDOW *wmedc;
	PANEL *pmedc;
	sqlite3_stmt *stmt;

	wmedc = newwin(20, 63, LINES / 2, (COLS - 40) / 2);
	keypad(wmedc, TRUE);
	pmedc = new_panel(wmedc);
	update_panels();
	doupdate();

	sqlite3_prepare_v2(db, REQORD_AMOUNT_REQUEST, strlen(REQORD_AMOUNT_REQUEST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, cardid);
	sqlite3_step(stmt);
	rc = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);

	sqlite3_prepare_v2(db, MEDCARD_REQUEST, strlen(MEDCARD_REQUEST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, cardid);
	sqlite3_step(stmt);

	do {
DRAW:			werase(wmedc);	
				box(wmedc, 0, 0);
				temp = sqlite3_column_text(stmt, 0);
				t = temp[0];
				mvwprintw(wmedc, 2, 3, "Тип записи: %s", 
						t == 'V' ? "Визит": (t == 'T' ? "Лечение": "Анализы"));
				mvwprintw(wmedc, 3, 3, "Таб. номер врача: %s", sqlite3_column_text(stmt, 1));
				mvwprintw(wmedc, 4, 3, "ФИО отвественного врача: %s", sqlite3_column_text(stmt, 2));
				mvwprintw(wmedc, 5, 3, "Должность врача: %s", sqlite3_column_text(stmt, 3));
				mvwprintw(wmedc, 6, 3, "Дата внесения записи: %s", sqlite3_column_text(stmt, 5));
				mvwprintw(wmedc, 7, 3, "%s: %s", 
						t == 'V' ? "Цель визита": (t == 'T' ? "Заболевание": "Вид анализов"), 
						sqlite3_column_text(stmt, 6));
				mvwprintw(wmedc, 8, 3, "%s %s", 
						t == 'V' ? "": (t == 'T' ? "Назначенное лечение:": "Результаты анализов:"), 
						sqlite3_column_text(stmt, 7));
				mvwprintw(wmedc, 18, 30, "%d/%d", i + 1, rc);
				while ((t = wgetch(wmedc)) != KEY_F(1)) 
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
	delwin(wmedc);
	del_panel(pmedc);
	update_panels();
	doupdate();
	return;
}

/* utility functions */

void bind_windows(PANEL **pmedcards, WINDOW **wmedcards, int cards) 
{
	int height = 15, width = 40, ypos = LINES - 20, xpos = COLS - 45;
	int i = 0;

	while (i < cards) {
		wmedcards[i] = newwin(height, width, ypos + 2 * i, xpos + 2 * i);
		box(wmedcards[i++], 0, 0);
	}

	i = 0;
	while (i < cards) {
		pmedcards[i] = new_panel(wmedcards[i]);
		i++;
	}
}

int get_card_amount(int regid)
{
	int amount = 7;	
	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(db, CARD_AMOUNT_REQUEST, strlen(CARD_AMOUNT_REQUEST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);
	sqlite3_step(stmt);
	amount = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);
	return amount;
}	

void card_populate(PANEL **pmedcards, WINDOW **wmedcards, int regid)
{
	int i = 0, *userptr;
	const char *sql = "SELECT * FROM patient_medcard where regid = ?";

	sqlite3_stmt *stmt;

	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);

	while(sqlite3_step(stmt) != SQLITE_DONE) {
		mvwprintw(wmedcards[i], 1, 2, "Карта № MC3070%d", sqlite3_column_int(stmt, 1));
		mvwaddch(wmedcards[i], 2, 0, ACS_LTEE);
		mvwhline(wmedcards[i], 2, 1, ACS_HLINE, 38);
		mvwaddch(wmedcards[i], 2, 39, ACS_RTEE);
		mvwprintw(wmedcards[i], 4, 2, "ФИО: %s", sqlite3_column_text(stmt, 2)); 
		mvwprintw(wmedcards[i], 5, 2, "Дата заведения: %s", sqlite3_column_text(stmt, 3)); 
		mvwprintw(wmedcards[i], 6, 2, "Тип медкарты: %s", sqlite3_column_text(stmt, 4)); 
		mvwprintw(wmedcards[i], 7, 2, "Количество записей: %s", sqlite3_column_text(stmt, 5)); 

		userptr = malloc(sizeof(int));
		*userptr = sqlite3_column_int(stmt, 1);

		set_panel_userptr(pmedcards[i], userptr);
		++i;
	}
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

