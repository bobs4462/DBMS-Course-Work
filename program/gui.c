#define GUI_H
#include <gui.h>

user_t login(char **credentials)
{
	user_t ret_val; //return value, defines type of user being authenticated
	WINDOW *login; 
	FIELD *fields[3]; //form fields

	int i = 0, y = (LINES - LOG_HIGHT) / 2, x = (COLS - LOG_WIDTH) / 2; //where to print user prompts
	int rows, cols, tn = 0;
	char tbuf[100];
	refresh();
	fields[0] = new_field(1,25, 1, 1, 0, 0);
	fields[1] = new_field(1,25, 2, 1, 0, 0);
	fields[2] = NULL;

	/* Setting form field attributes */
	set_field_opts(fields[0], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	set_field_opts(fields[1], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	set_field_back(fields[0], A_UNDERLINE);
	set_field_back(fields[1], A_UNDERLINE);

	FORM *login_form = new_form(fields); //creating new form using the fields
	scale_form(login_form, &rows, &cols); //calculating necessary size for login form
	login = newwin(rows + 3, cols + 11, y,  x); //special windows used for form
    keypad(login, TRUE); //let every keystroke through
	form_opts_off(login_form, O_BS_OVERLOAD); //neccessary in order to prevent strange behaviour of backspace key

	set_form_win(login_form, login); //binding form to window
	set_form_sub(login_form, derwin(login, rows, cols, 2, 8));
	box(login, 0, 0); //visual stuff
	mvwprintw(login,  1, (cols + 11) / 2 - 2, "ВХОД");
	mvwprintw(login,  3, 2, "ЛОГИН: ");
	mvwprintw(login, 4, 1, "ПАРОЛЬ: ");
	post_form(login_form);
	wrefresh(login);
	while ((i = wgetch(login)) != KEY_F(1)) //control loop to manage form 
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
				break;
			case 10:
				tbuf[tn] = '\0';
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
	credentials[0] = malloc(strlen(field_buffer(fields[0], 0)) + 1); //extracting inforamation from form
	credentials[1] = malloc(++tn);
	strcpy(credentials[0], field_buffer(fields[0], 0));
	strcpy(credentials[1], tbuf);
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

void patient_interface(sqlite3 *db)
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
	free(menu_items);
	free_menu(main_menu);
}

void doctor_interface(sqlite3 *db)
{
	return;
}

void registry_interface(sqlite3 *db)
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
void medical_cards(int regid, sqlite3 db)
{
	char *sql;
	sqlite3_mprintf(sql, "select count(*) from medcard where regid = %q", regid);
	sqlite3_get_table(sql)	
	PANEL * 
	mvprintw(LINES - 1, 2, "SUCCESS");
	refresh();
}
