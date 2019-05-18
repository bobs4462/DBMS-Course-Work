#include <patient.h>

void patient_interface(int regid) // the boss function to manage the view of interface
{
	sqlite3_stmt *stmt;
	char temp[1000];

	int i = 0, rows, cols;

	WINDOW *win_main_menu;

	PANEL *menu;
	MENU *main_menu;
	char *choices[] = {
		"Запись к врачу                ",
		"Просмотр своих записей        ",
		"Просмотр выписанных рецептов  ",
		"Расписание врачей             ",
		"Медицинские книжки            ",
		"Смена пароля                  ",
		"Выход                         ",
	};

	ITEM **menu_items; 

	init_menu(&menu_items,
			choices, 
			sizeof(choices) / sizeof(char *));
	main_menu = new_menu((ITEM **)menu_items);
	scale_menu(main_menu, &rows, &cols);

	win_main_menu = newwin(rows + 4, cols + 4, 4, 75); 
	set_menu_win(main_menu, win_main_menu); 
	set_menu_sub(main_menu,
			derwin(win_main_menu, rows, cols, 2, 2)); 
	menu = new_panel(win_main_menu);
	struct win_pan *pat = patient_info(regid);


	keypad(win_main_menu, TRUE); 
	box(win_main_menu, 0, 0);

	post_menu(main_menu);
	update_panels();

	doupdate();
	while((i = wgetch(win_main_menu)) != KEY_F(2)){ 
		switch(i) {	
	case KEY_DOWN:
		menu_driver(main_menu, REQ_DOWN_ITEM);
		break;
	case KEY_UP:
		menu_driver(main_menu, REQ_UP_ITEM);
		break;
	case 10:
		if (current_item(main_menu) == menu_items[0])
			appointment(regid);
		else if (current_item(main_menu) == menu_items[1])
			show_appointments(regid);
		else if (current_item(main_menu) == menu_items[2])
			show_receipts(regid);
		else if (current_item(main_menu) == menu_items[3])
			timetable(0);
		else if (current_item(main_menu) == menu_items[4])
			medical_cards(regid, 0);
		else if (current_item(main_menu) == menu_items[5])
			password_change(regid);
		else
			goto EXIT;
		}
	}	

EXIT:
	i = 0;
	while(*(menu_items + i)) {
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
	free(pat);
}
