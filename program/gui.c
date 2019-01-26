#define GUI_H
#include <gui.h>

void login(void)
{
	setlocale(LC_ALL, ""); // To make program locale aware, doesn't display cyrillic without it

	WINDOW *login;
	FIELD *fields[3]; 
	initscr();             // Starting ncurses
	cbreak();
	noecho();

	int i = 0, y = (LINES - LOG_HIGHT) / 2, x = (COLS - LOG_WIDTH) / 2; //where to print user prompts
	int rows, cols, tn = 0;
	char tbuf[100];
	refresh();
	fields[0] = new_field(1,25, 1, 1, 0, 0);
	fields[1] = new_field(1,25, 2, 1, 0, 0);
	fields[2] = NULL;
	field_opts_off(fields[0], O_AUTOSKIP);
	field_opts_off(fields[0], O_BLANK);
	field_opts_off(fields[1], O_BLANK);
	field_opts_off(fields[1], O_AUTOSKIP);
	field_opts_off(fields[0], O_STATIC);
	field_opts_off(fields[1], O_STATIC);
	set_field_back(fields[0], A_UNDERLINE);
	set_field_back(fields[1], A_UNDERLINE);

	FORM *login_form = new_form(fields);
	scale_form(login_form, &rows, &cols);
	login = newwin(rows + 2, cols + 11, y,  x);
    keypad(login, TRUE);
	form_opts_off(login_form, O_BS_OVERLOAD);

	set_form_win(login_form, login);
	set_form_sub(login_form, derwin(login, rows, cols, 1, 8));
	box(login, 0, 0);
	mvwprintw(login,  2, 1, "ЛОГИН: ");
	mvwprintw(login, 3, 1, "ПАРОЛЬ: ");
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
			case KEY_DC:
			case KEY_BACKSPACE:
				form_driver(login_form, REQ_DEL_PREV);
				break;
			case 10:
				tbuf[tn] = '\0';
				mvprintw(1,1, "%s", field_buffer(fields[0], 0));
				mvprintw(2,1, "%s", tbuf, 0);
				goto exit;
			default:
				if (current_field(login_form) == *(fields + 1)) {
					tbuf[tn++] = i;
					form_driver(login_form, '*');
				}
				else
					form_driver(login_form, i);
				//form_driver(login_form, REQ_CLR_EOL);
				break;
		}
exit: getch();

	endwin();
}
