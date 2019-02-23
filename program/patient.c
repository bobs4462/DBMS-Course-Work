#define GUI_H
#include <gui.h>
#include <sqlite_adapter.h>

void patient_interface(int regid)
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

	win_main_menu = newwin(rows + 4, cols + 4, 4, 55); 
	set_menu_win(main_menu, win_main_menu); 
	set_menu_sub(main_menu, derwin(win_main_menu, rows, cols, 2, 2)); 
	menu = new_panel(win_main_menu);
	patient_info = newwin(30, 50, 4, 4);
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

	update_panels();
	post_menu(main_menu);

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
					appointement();
				else if (current_item(main_menu) == menu_items[1])
					timetable();
				else if (current_item(main_menu) == menu_items[2])
					medical_cards();
		}
	}	
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

void init_menu(ITEM ***some_items, char **choices, size_t n_choices)
{
	int i = -1;
	*some_items = (ITEM**)calloc(n_choices + 1, sizeof(ITEM *));
 	while(++i < n_choices) 
		(*some_items)[i] = new_item(choices[i], NULL);
	(*some_items)[n_choices] = NULL;
}

void appointement(void)
{
	mvprintw(LINES - 1, 2, "SUCCESS");
	refresh();
}

void timetable(void)
{
	mvprintw(LINES - 1, 2, "SUCCESS");
	refresh();
}

void medical_cards(int regid)
{
	int rows, cols, i = 0;
	int cards = get_card_amount(regid);

	PANEL *pmedcards[cards];
	WINDOW *wmedcards[cards];
	bind_windows(pmedcards, wmedcards, cards);
	card_populate(wmedcards, regid);
	refresh();

	i = 0;
	while (i < cards)
		delwin(wmedcards[i]);

	i = 0;
	while (i < cards)
		del_panel(wmedcards[i]);
	return;
}

void bind_windows(PANEL **pmedcards, WINDOW **wmedcards, int cards) 
{
	int height = 30, width = 40, ypos = LINES - 30, xpos = WIDTH / 2 - 15;
	int i = 0;
	while (i < cards) 
		wmedcards[i] = newwin(height, width, ypos, xpos);

	i = 0;
	while (i < cards) 
		pmedcards[i] = new_panel(wmedcards[i]);
}

void card_populate(WINDOW **wmedcards, int regid)
{
	int i = 0;
	const char *sql = "SELECT * FROM medicalcard where card GLOB '*0' || ?";
	sqlite3_stmt *stmt;
	const char *tail;
	sqlite3_prepare_v2(db, CARD_AMOUNT_REQUEST, strlen(CARD_AMOUNT_REQUEST), &stmt, &tail);
	sqlite3_bind_int(stmt, 1, regid);
	while(sqlite3_step(stmt) != SQLITE_DONE) {
		//TODO fill up cards with data
}


