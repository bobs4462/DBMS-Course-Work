#define GUI_H
#include <gui.h>

user_t login(char **credentials)
{
	//setlocale(LC_ALL, ""); // To make program locale aware, doesn't display cyrillic without it

	user_t ret_val;
	WINDOW *login;
	FIELD *fields[3]; 

	int i = 0, y = (LINES - LOG_HIGHT) / 2, x = (COLS - LOG_WIDTH) / 2; //where to print user prompts
	int rows, cols, tn = 0;
	char tbuf[100];
	refresh();
	fields[0] = new_field(1,25, 1, 1, 0, 0);
	fields[1] = new_field(1,25, 2, 1, 0, 0);
	fields[2] = NULL;

	set_field_opts(fields[0], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	set_field_opts(fields[1], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	set_field_back(fields[0], A_UNDERLINE);
	set_field_back(fields[1], A_UNDERLINE);

	FORM *login_form = new_form(fields);
	scale_form(login_form, &rows, &cols);
	login = newwin(rows + 3, cols + 11, y,  x);
    keypad(login, TRUE);
	form_opts_off(login_form, O_BS_OVERLOAD);

	set_form_win(login_form, login);
	set_form_sub(login_form, derwin(login, rows, cols, 2, 8));
	box(login, 0, 0);
	mvwprintw(login,  1, (cols + 11) / 2 - 2, "ВХОД");
	mvwprintw(login,  3, 2, "ЛОГИН: ");
	mvwprintw(login, 4, 1, "ПАРОЛЬ: ");
	post_form(login_form);
	wrefresh(login);
	while ((i = wgetch(login)) != KEY_F(1))
		switch(i) {
			case KEY_UP:
				form_driver(login_form, REQ_PREV_FIELD);
				form_driver(login_form, REQ_END_LINE);
				break;
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
			case '\t':
				form_driver(login_form, REQ_NEXT_FIELD);
				form_driver(login_form, REQ_END_LINE);
				break;
			case KEY_DC:
			case KEY_BACKSPACE:
				form_driver(login_form, REQ_DEL_PREV);
				break;
			case 10:
				tbuf[tn] = '\0';
				mvprintw(1,1, "%s", field_buffer(fields[0], 0));
				mvprintw(2,1, "%s", tbuf);
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
				goto EXIT;
			default:
				if (current_field(login_form) == *(fields + 1)) {
					tbuf[tn++] = i;
					form_driver(login_form, '*');
				}
				else
					form_driver(login_form, i);
				break;
		}
EXIT:
	credentials[0] = malloc(strlen(field_buffer(fields[0], 0)) + 1);
	credentials[1] = malloc(strlen(tbuf) + 1);
	strcpy(credentials[0], field_buffer(fields[0], 0));
	strcpy(credentials[1], tbuf);
	i = 0;
	while(*(fields + i))
		free_field(fields[i++]);
	unpost_form(login_form);
	free_form(login_form);
	delwin(login);
	erase();
	return ret_val;
}

void patient_interface(void)
{
	//initscr();
	int i = 0;
	WINDOW *win_main_menu;
	MENU *main_menu;
	char *choices[] = {
						"Запись к врачу",
						"Расписание врачей",
						"Медицинские книжки",
						"GH"
						
					  };
	ITEM **menu_items; 
	init_menu(&menu_items, choices, sizeof(choices) / sizeof(char *));
	main_menu = new_menu((ITEM **)menu_items);

	post_menu(main_menu);
	refresh();
	keypad(stdscr, TRUE);
	while((i = getch()) != KEY_F(1))
	{   switch(i)
	    {	case KEY_DOWN:
		        menu_driver(main_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(main_menu, REQ_UP_ITEM);
				break;
		}
	}	
	i = 0;
	while(*(menu_items + i)) {
		free_item(*(menu_items + i++));
	}
	free(menu_items);
	free_menu(main_menu);
}

void init_menu(ITEM ***some_items, char **choices, size_t n_choices)
{
	int i = -1;
	*some_items = (ITEM**)calloc(n_choices + 1, sizeof(ITEM *));
 	while(++i < n_choices) 
		(*some_items)[i] = new_item(choices[i], choices[i]);
	(*some_items)[n_choices] = NULL;
}

