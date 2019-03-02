#include <login.h>

user_t login(int *regid)
{
	user_t ret_val; //return value, defines type of user being authenticated
	curs_set(2);
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
	mvwhline(login, 2, 1, ACS_HLINE, cols + 9);
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
	curs_set(0);
	return ret_val;
}

int get_regid(char *login)
{
	int i = 1;
	while(login[i++] == '0');
	return atoi(login + (--i));
}


int pass_verify(char *login, char *pass)
{
	int regid, i = 1;
	while (login[i++] == '0');
	regid = atoi(login + (--i));
	return authenticate(regid, pass);
}

int authenticate(int regid, char *pass)
{
	int rv;
	sqlite3_stmt *stmt;
	const char *tail;
	sqlite3_prepare_v2(db, PASS_REQUEST, strlen(PASS_REQUEST), &stmt, &tail);
	sqlite3_bind_int(stmt, 1, regid);
	sqlite3_bind_text(stmt, 2, pass, strlen(pass), SQLITE_TRANSIENT);
	if (sqlite3_step(stmt) == SQLITE_ROW)
		rv = 0;
	else {
		rv = 1;
	}
	sqlite3_finalize(stmt);
	return rv;
}
