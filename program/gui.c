#define GUI_H
#include <gui.h>
#include <sqlite_adapter.h>

user_t login(int regid)
{
	user_t ret_val; //return value, defines type of user being authenticated
	WINDOW *login; 
	FIELD *fields[3]; //form fields
	FORM *login_form;

	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_CYAN, COLOR_BLACK);
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(5, COLOR_BLUE, COLOR_BLACK);

	int i = 0, y = (LINES - 8) / 2, x = (COLS - 41) / 2; //where to print user prompts
	int rows, cols, tn = 0;
	char tbuf[100];

	fields[0] = new_field(1,30, 2, 1, 0, 0);
	fields[1] = new_field(1,30, 4, 1, 0, 0);
	fields[2] = NULL;

	/* Setting form field attributes */
	set_field_opts(fields[0], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	set_field_opts(fields[1], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	set_field_back(fields[0], A_UNDERLINE);
	set_field_back(fields[1], A_UNDERLINE);

	login_form = new_form(fields); //creating new form using the fields
	form_opts_off(login_form, O_BS_OVERLOAD); //neccessary in order to prevent strange behaviour of backspace key

	scale_form(login_form, &rows, &cols); //calculating necessary size for login form

	refresh();

	login = newwin(rows + 5, cols + 11, y,  x); //special windows used for form
    keypad(login, TRUE); //let every keystroke through

	set_form_win(login_form, login); //binding form to window
	set_form_sub(login_form, derwin(login, rows, cols, 3, 8));
	box(login, 0, 0); //visual stuff

	mvwaddch(login, 2, 0, ACS_LTEE);
	mvwhline(login, 2, 1, ACS_HLINE, cols + 11);
	mvwaddch(login, 2, cols + 10, ACS_RTEE);
	wattron(login, COLOR_PAIR(BLUE));
	mvwprintw(login,  1, (cols + 11) / 2 - 2, "ВХОД");
	mvwprintw(login,  5, 2, "ЛОГИН: ");
	mvwprintw(login, 7, 1, "ПАРОЛЬ: ");
	wattroff(login, COLOR_PAIR(GREEN));

	post_form(login_form);
	wrefresh(login);

	while (i = wgetch(login)) //control loop to manage form 
		switch(i) {
			case KEY_UP:
				form_driver(login_form, REQ_PREV_FIELD);
				form_driver(login_form, REQ_END_LINE);
				break;
			case '\t':
			case KEY_DOWN:
				form_driver(login_form, REQ_NEXT_FIELD);
				form_driver(login_form, REQ_END_LINE);
				break;
			case KEY_LEFT:
				form_driver(login_form, REQ_LEFT_CHAR);
				break;
			case KEY_RIGHT:
				form_driver(login_form, REQ_RIGHT_CHAR);
				break;
			case KEY_DC:
			case KEY_BACKSPACE:
				form_driver(login_form, REQ_DEL_PREV);
				if (current_field(login_form) == *(fields + 1))
					if (tn > 0)
						--tn;
				break;
			case 10:
				tbuf[tn] = '\0';
				if (pass_verify(field_buffer(fields[0], 0), tbuf)) {
					mvprintw(LINES - 5, (COLS - strlen(LOG_INCORRECT) / 2) / 2, LOG_INCORRECT);
					form_driver(login_form, REQ_END_LINE);
					refresh();
					continue;
				}
				switch(field_buffer(fields[0], 0)[0]) {
					case 'P':
						ret_val = PATIENT;
						break;
					case 'D':
						ret_val = DOCTOR;
						break;
					case 'R':
						ret_val = REGISTRY;
						break;
					default:
						ret_val = UNKNOWN;
						break;
				}
				goto EXIT; // I know this is bad, but more ellegant than other methods
			default:
				if (current_field(login_form) == *(fields + 1)) { //if the field is for password, then put '*' instead of echoing the input
					tbuf[tn++] = i;
					form_driver(login_form, '*');
				}
				else
					form_driver(login_form, i); //otherwise echo input
				break;
		}
EXIT:
	*regid = get_regid(field_buffer(fields[0], 0));	
	i = 0;
	while(*(fields + i)) //cleaning up
		free_field(fields[i++]);
	unpost_form(login_form);
	free_form(login_form);
	delwin(login);
	erase();
	refresh();
	return ret_val;
}

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

void doctor_interface(void)
{
	return;
}

void registry_interface(void)
{
	return;
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

void medical_cards()
{
	int rows, cols;
	
	PANEL *medcards[]
	refresh();
return;
}

int pass_verify(char *login, char *pass)
{
	int regid, i = 1;
	while (login[i++] == '0');
	regid = atoi(login + (--i));
	return authenticate(regid, pass);
}

int get_regid(char *login)
{
	int i = 0;
	while(login[i++] = '0');
	return atoi(login + (--i));
}
