#include <patient.h>

void patient_interface(int regid) // the boss function to manage the view of interface
{
	const char *sql = "SELECT * from name;";
	char **result, *err, temp[1000];

	int i = 0, rows, cols;
	WINDOW *win_main_menu, *patient_info;
	FORM *info_form;
	FIELD *info_fields[8];
	info_fields[7] = NULL;
	for (i = 0; i < 14; i += 2) {
		info_fields[i / 2] = new_field(2, 40, i, 4, 1, 1);
		set_field_opts(info_fields[i / 2], ~(O_ACTIVE));
	}		
	i = 0;
	info_form = new_form(info_fields);


	PANEL *menu, *patient;
	MENU *main_menu;
	char *choices[] = {
						"Запись к врачу",
						"Расписание врачей",
						"Медицинские книжки",
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
	patient_info = newwin(30, 70, 4, 4);
	scale_form(info_form, &rows, &cols);
	set_form_win(info_form, patient_info);
	set_form_sub(info_form, derwin(patient_info, rows, cols, 3, 2));
	post_form(info_form);
	patient = new_panel(patient_info);
	sqlite3_get_table(db, sql, &result, &rows, &cols, &err);
	
	for (i = 0; i < cols; ++i) {
		sprintf(temp, "%s : %s", result[i], result[i + 7]); 
		set_field_buffer(info_fields[i], 0, temp);
	}
	i = 0;
	sqlite3_free_table(result);
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
				if (current_item(main_menu) == menu_items[0])
					appointement(regid);
				else if (current_item(main_menu) == menu_items[1])
					timetable(regid);
				else if (current_item(main_menu) == menu_items[2])
					medical_cards(regid);
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
	while(*(info_fields + i)) { //cleaning up
		free_field(*(info_fields + i++));
	}
	free(menu_items);
	free_menu(main_menu);
	free_form(info_form);
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

void timetable(int regid) //doctor's timetable view
{
	mvprintw(LINES - 1, 2, "SUCCESS");
	refresh();
}

void medical_cards(int regid) //work with patient's medical cards
{
	int rows, cols, i = 0, j = 0;
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
	}
  			
	i = 0;
	while (i < cards) {
		free((int *)panel_userptr(pmedcards[j]));
		del_panel(pmedcards[i]);
		wclear(wmedcards[i]);
		wrefresh(wmedcards[i]);
		delwin(wmedcards[i++]);
	}


	return;
}

/* utility functions */

void bind_windows(PANEL **pmedcards, WINDOW **wmedcards, int cards) 
{
	int height = 15, width = 40, ypos = LINES - 20, xpos = COLS - 45;
	int i = 0;

	while (i < cards) {
		wmedcards[i] = newwin(height, width, ypos, xpos + i);
		box(wmedcards[i++], 0, 0);
	}

	i = 0;
	while (i < cards) {
		pmedcards[i] = new_panel(wmedcards[i]);
		i++;
	}
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

void show_card(int cardid)
{
	int t = 0, i = 1, j = 0;
    const char *temp;
	WINDOW *wmedc;
	PANEL *pmedc;
	sqlite3_stmt *stmt;

	wmedc = newwin(20, 40, LINES / 2, (COLS - 40) / 2);
	keypad(wmedc, TRUE);
	pmedc = new_panel(wmedc);
	update_panels();
	doupdate();

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
				mvwprintw(wmedc, 5, 3, "Должность врача :%s", sqlite3_column_text(stmt, 3));
				mvwprintw(wmedc, 6, 3, "Дата внесения записи: %s", sqlite3_column_text(stmt, 5));
				mvwprintw(wmedc, 7, 3, "%s: %s", 
						t == 'V' ? "Цель визита": (t == 'T' ? "Заболевание": "Вид анализов"), 
						sqlite3_column_text(stmt, 6));
				mvwprintw(wmedc, 8, 3, "%s: %s", 
						t == 'V' ? "\021": (t == 'T' ? "Назначенное лечение": "Результаты анализов"), 
						sqlite3_column_text(stmt, 7));
				while ((t = wgetch(wmedc)) != KEY_F(1)) 
					switch (t) {
						case 'l':
						case KEY_RIGHT:
							if (sqlite3_step(stmt) == SQLITE_DONE) {
								sqlite3_reset(stmt);
								for (int k = 0; k <= i; ++k)
									sqlite3_step(stmt);
							}
							else
								++i;
							break;
						case 'h':
						case KEY_LEFT:
							if (0 == i)
								break;
							sqlite3_reset(stmt);
							for (j = 0; j < i; ++j) {
								sqlite3_step(stmt);
							}
							i = j;
							goto DRAW; //244
					}
	} while(TRUE); //TODO FIX the bug, while is not reached
	sqlite3_finalize(stmt);
	delwin(wmedc);
	del_panel(pmedc);
	update_panels();
	doupdate();
	return;
}
