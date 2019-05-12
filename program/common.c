#include <common.h>

int trim (char *array);
int message_box(const char *text, char *header, int y, int x, int h, int w, int buttons) 
{
	WINDOW *message_win, *message_sub;
	y = y > 0 ? y : (LINES / 2 - (h + 4) / 2);
	x = x > 0 ? x : (COLS / 2 - (w + 4) / 2);

	PANEL *message_pan;
	message_win = newwin(h + 4, w + 4, y, x);
	message_pan = new_panel(message_win);
	int offset = 1;
	if (header) {
		mvwaddch(message_win, 2, 0, ACS_LTEE);
		mvwhline(message_win, 2, 1, ACS_HLINE, w + 6);
		mvwaddch(message_win, 2, w + 7, ACS_RTEE);
		mvwprintw(message_win, 1, 2, header);
		offset = 3;
	}
	message_sub = derwin(message_win, h, w, offset, 2);
	mvwprintw(message_sub, 1, 2, text);
	box(message_win, 0, 0);
	ITEM **ok = malloc(sizeof(ITEM *) * 3);
	MENU *response;
	if (buttons) {
		char *opts[3] = {
			"ДА",
			"НЕТ",
			NULL
		};
		ok[0] = new_item(opts[0], NULL);
		ok[1] = new_item(opts[1], NULL);
		ok[2] = NULL;
		response = new_menu(ok);
		set_menu_format(response, 1, 2);
		set_menu_win(response, message_win);
		set_menu_sub(response, derwin(message_win, 1, 10, h + 2, (w - 6) / 2));
		post_menu(response);
	}

	int retval = 0;
	update_panels();
	doupdate();
	keypad(message_win, TRUE);
	if (buttons) 
	while((y = wgetch(message_win)) != KEY_F(2))
	{
		switch(y)
		{
			case KEY_RIGHT:
				menu_driver(response, REQ_RIGHT_ITEM);
				break;
			case KEY_LEFT:
				menu_driver(response, REQ_LEFT_ITEM);
				break;
			case 10:
				if (current_item(response) == ok[0])
					retval = 1;
				unpost_menu(response);
				free_menu(response);
				goto CLEANUP;
		}
	}
	else
		wgetch(message_win);
CLEANUP:
	del_panel(message_pan);
	delwin(message_sub);
	delwin(message_win);
	update_panels();
	doupdate();
	return retval;
}

int show_menu(char **items, int sz, char *msg, int yc, int xc)
{
	int x, y;
	MENU *menu;
	WINDOW *wmenu, *sub;
	PANEL *pmenu;
	ITEM *itptr[sz];
	for (int i = 0, *uptr; items[i]; i++) {
		itptr[i] = new_item(items[i], NULL);
		uptr = malloc(sizeof(int));	
		*uptr = i;
		set_item_userptr(itptr[i], uptr);
	}
	itptr[sz - 1] = NULL;
	menu = new_menu(itptr);
	scale_menu(menu, &y, &x);
	yc = yc > 0 ? yc : (LINES - y - 8) / 2;
	xc = xc > 0 ? xc : (COLS - x - 16) / 2;
	wmenu = newwin(y + 5, x + 12, yc, xc);
	keypad(wmenu, TRUE);
	set_menu_win(menu, wmenu);	
	set_menu_sub(menu, sub = derwin(wmenu, y, x , 4, 6));
	post_menu(menu);
	box(wmenu, 0, 0);
	mvwaddch(wmenu, 2, 0, ACS_LTEE);
	mvwhline(wmenu, 2, 1, ACS_HLINE, x + 10);
	mvwaddch(wmenu, 2, x + 11, ACS_RTEE);
	mvwprintw(wmenu, 1, (x + 12 - strlen(msg) / 2) / 2, msg);
	wrefresh(wmenu);
	pmenu = new_panel(wmenu);
	update_panels();
	doupdate();

	while((y = wgetch(wmenu)) != KEY_F(2))
	{
		switch(y)
		{
			case KEY_DOWN:
				menu_driver(menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(menu, REQ_UP_ITEM);
				break;
			case 10:
				x = *(int*)item_userptr(current_item(menu));
				goto CLEANUP;
		}
	}

CLEANUP:
	for (int i = 0; i < sz; i++) {
		free(item_userptr(itptr[i]));
		free_item(itptr[i]);
	}
	unpost_menu(menu);
	free_menu(menu);
	werase(wmenu);
	wrefresh(wmenu);
	del_panel(pmenu);
	delwin(sub);
	delwin(wmenu);
	update_panels();
	doupdate();
	return x;
}

char *search(char *header)
{
	char *input = calloc(1000, 1);
	curs_set(1);
	FORM *search_form;
	WINDOW *search_win, *search_sub;
	FIELD *search_field[4];
	search_field[0] = new_field(1, 58, 2, 9, 0, 0);
	search_field[1] = new_field(1, 30, 5, 35, 0, 0);
	search_field[2] = new_field(1, 40, 8, 29, 0, 0);
	search_field[3] = NULL;


	set_field_opts(search_field[0], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
	set_field_opts(search_field[1], ~(O_AUTOSKIP | O_BLANK | O_EDIT));
	set_field_opts(search_field[2], ~(O_AUTOSKIP | O_BLANK | O_EDIT));
	search_form = new_form(search_field);
	search_win = newwin(15, 80, LINES / 2 - 7, COLS / 2 - 40);

	form_opts_off(search_form, O_BS_OVERLOAD); //neccessary in order to prevent strange behaviour of backspace key
	keypad(search_win, TRUE); //let every keystroke through


	set_form_win(search_form, search_win); //binding form to window
	set_form_sub(search_form, search_sub = derwin(search_win, 11, 76, 3, 2));
	box(search_win, 0, 0); //visual stuff
	mvwaddch(search_win, 2, 0, ACS_LTEE);
	mvwhline(search_win, 2, 1, ACS_HLINE, 78);
	mvwaddch(search_win, 2, 79, ACS_RTEE);
	mvwprintw(search_win, 1, 40 - strlen(header) / 4, header);
	post_form(search_form);
	mvwaddch(search_sub, 1, 8, ACS_ULCORNER);
	mvwhline(search_sub, 1, 9, ACS_HLINE, 58);
	mvwaddch(search_sub, 1, 67, ACS_URCORNER);
	mvwaddch(search_sub, 3, 8, ACS_LLCORNER);
	mvwhline(search_sub, 3, 9, ACS_HLINE, 58);
	mvwaddch(search_sub, 3, 67, ACS_LRCORNER);
	mvwvline(search_sub, 2, 8, ACS_VLINE, 1);
	mvwvline(search_sub, 2, 67, ACS_VLINE, 1);
	wrefresh(search_win);
	set_field_buffer(search_field[1], 0, "Найти");
	set_field_buffer(search_field[2], 0, "Выбрать из списка");

	int i = 0;

	while (i = wgetch(search_win)) //control loop to manage form 
		switch(i) {
			case KEY_UP:
				form_driver(search_form, REQ_PREV_FIELD);
				form_driver(search_form, REQ_END_LINE);
				if (current_field(search_form) == search_field[1]) {
					set_field_fore(search_field[1], COLOR_PAIR(WHITE));
					set_field_fore(search_field[2], A_NORMAL);
				}
				else if (current_field(search_form) == search_field[2]) {
					set_field_fore(search_field[2], COLOR_PAIR(WHITE));
					set_field_fore(search_field[1], A_NORMAL);
				}
				else {
					set_field_fore(search_field[2], A_NORMAL);
					set_field_fore(search_field[1], A_NORMAL);
				}
				break;
			case '\t':
			case KEY_DOWN:
				form_driver(search_form, REQ_NEXT_FIELD);
				form_driver(search_form, REQ_END_LINE);
				if (current_field(search_form) == search_field[1]) {
					set_field_fore(search_field[1], COLOR_PAIR(WHITE));
					set_field_fore(search_field[2], A_NORMAL);
				}
				else if (current_field(search_form) == search_field[2]) {
					set_field_fore(search_field[2], COLOR_PAIR(WHITE));
					set_field_fore(search_field[1], A_NORMAL);
				}
				else {
					set_field_fore(search_field[2], A_NORMAL);
					set_field_fore(search_field[1], A_NORMAL);
				}

				break;
			case KEY_LEFT:
				form_driver(search_form, REQ_LEFT_CHAR);
				break;
			case KEY_RIGHT:
				form_driver(search_form, REQ_RIGHT_CHAR);
				break;
			case KEY_DC:
			case KEY_BACKSPACE:
			case 127:
				if (current_field(search_form) == search_field[0])
					form_driver(search_form, REQ_DEL_PREV);
				break;
			case 10:
				if ((current_field(search_form) == search_field[1]) || (current_field(search_form) == search_field[0])) {
					form_driver(search_form, REQ_NEXT_FIELD);
					form_driver(search_form, REQ_END_LINE);
					strcpy(input, field_buffer(search_field[0], 0));
				}
				else
					strcpy(input, "%");
				goto EXIT;

			default:
				if (current_field(search_form) == search_field[0])
					form_driver(search_form, i); //otherwise echo input
				break;
		}
EXIT:
	delwin(search_sub);
	delwin(search_win);
	unpost_form(search_form);
	curs_set(0);
	clear();
	strip(input);
	return input;
}

void strip(char *text)
{
	int len = strlen(text);
	while (text[--len] == ' ')
		text[len] = 0;
}

int medical_cards(int regid, int EXTRACT) //work with patient's medical cards
{
	int i = 0, j = 0;
	int cards = get_card_amount(regid);
	j = cards - 1;	

	PANEL **pmedcards = malloc(sizeof(PANEL *) * cards);
	WINDOW **wmedcards = malloc(sizeof(WINDOW *) * cards);
	WINDOW **subs = malloc(sizeof(WINDOW *) * cards);

	bind_windows(pmedcards, wmedcards, subs, cards);

	card_populate(pmedcards, wmedcards, subs, regid);

	update_panels();
	doupdate();
	while ((i = getch()) != 'q') {
		switch(i) {
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
				if (EXTRACT) {
					j =  *((int *)panel_userptr(pmedcards[j]));
					goto EXIT;
				}
				show_card(*((int *)panel_userptr(pmedcards[j])));
		}
		update_panels();
		doupdate();
	}
  			
EXIT:
	i = 0;
	while (i < cards) {
		free((int *)panel_userptr(pmedcards[i]));
		del_panel(pmedcards[i]);
		wclear(wmedcards[i]);
		wrefresh(wmedcards[i]);
		delwin(subs[i]);
		delwin(wmedcards[i++]);
	}
	update_panels();
	doupdate();

	return j;
}

void card_populate(PANEL **pmedcards, WINDOW **wmedcards, WINDOW **subs, int regid)
{
	int i = 0, *userptr;
	const char *sql = "SELECT * FROM patient_medcard where regid = ?";

	sqlite3_stmt *stmt;

	sqlite3_prepare_v2(db, sql, strlen(sql), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);

	while(sqlite3_step(stmt) != SQLITE_DONE) {
		mvwprintw(wmedcards[i], 1, 2, "Карта № MC3070%d", sqlite3_column_int(stmt, 1));
		mvwaddch(wmedcards[i], 2, 0, ACS_LTEE);
		mvwhline(wmedcards[i], 2, 1, ACS_HLINE, 48);
		mvwaddch(wmedcards[i], 2, 49, ACS_RTEE);
		mvwprintw(subs[i], 4, 2, "ФИО: %s", sqlite3_column_text(stmt, 2)); 
		mvwprintw(subs[i], 5, 2, "Дата заведения: %s", sqlite3_column_text(stmt, 3)); 
		mvwprintw(subs[i], 6, 2, "Тип медкарты: %s", sqlite3_column_text(stmt, 4)); 
		mvwprintw(subs[i], 7, 2, "Количество записей: %s", sqlite3_column_text(stmt, 5)); 

		userptr = malloc(sizeof(int));
		*userptr = sqlite3_column_int(stmt, 1);

		set_panel_userptr(pmedcards[i], userptr);
		++i;
	}
	sqlite3_finalize(stmt);
}

void show_card(int cardid)
{
	int t = 0, i = 0, j = 0, c;
	int rc; //record count
    const char *temp;
	sqlite3_stmt *stmt;

	sqlite3_prepare_v2(db, REQORD_AMOUNT_REQUEST, strlen(REQORD_AMOUNT_REQUEST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, cardid);
	sqlite3_step(stmt);
	rc = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);
	if (!rc)
		return;

	WINDOW *wmedc, *sub;
	PANEL *pmedc;

	wmedc = newwin(15, 80, LINES / 2 - 1, (COLS - 70) / 2);
	sub = derwin(wmedc, 13, 72, 1, 3);
	box(wmedc, 0, 0);
	keypad(wmedc, TRUE);
	pmedc = new_panel(wmedc);
	update_panels();
	doupdate();

	sqlite3_prepare_v2(db, MEDCARD_REQUEST, strlen(MEDCARD_REQUEST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, cardid);
	sqlite3_step(stmt);
	int row = 0, y, x;

	do {
DRAW:			werase(sub);	
				row = 1;
				temp = sqlite3_column_text(stmt, 0);
				t = temp[0];
				mvwprintw(sub, row++, 0, "Тип записи: %s", 
						t == 'V' ? "Визит": (t == 'T' ? "Лечение": "Анализы"));
				mvwprintw(sub, row++, 0, "Таб. номер врача: %s", sqlite3_column_text(stmt, 1));
				mvwprintw(sub, row++, 0, "ФИО отвественного врача: %s", sqlite3_column_text(stmt, 2));
				mvwprintw(sub, row++, 0, "Должность врача: %s", sqlite3_column_text(stmt, 3));
				mvwprintw(sub, row++, 0, "Дата внесения записи: %s", sqlite3_column_text(stmt, 5));
				mvwprintw(sub, row++, 0, "%s: %s", 
						t == 'V' ? "Цель визита": (t == 'T' ? "Заболевание": "Вид анализов"), 
						sqlite3_column_text(stmt, 6));
				getyx(sub, y, x);
				mvwprintw(sub, y + 1, 0, "%s %s", 
						t == 'V' ? "": (t == 'T' ? "Назначенное лечение:": "Результаты анализов:"), 
						sqlite3_column_text(stmt, 7));
				wmove(sub, 11, 33); wclrtoeol(sub);
				mvwprintw(sub, 12, 33, "%d/%d", i + 1, rc);
				wrefresh(sub);
				while ((t = wgetch(wmedc)) != 'q') 
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
	del_panel(pmedc);
	delwin(sub);
	delwin(wmedc);
	update_panels();
	doupdate();
	return;
}

/* utility functions */

void bind_windows(PANEL **pmedcards, WINDOW **wmedcards, WINDOW **subs, int cards) 
{
	int height = 15, width = 50, ypos = LINES - 25, xpos = COLS - 75;
	int i = 0;

	while (i < cards) {
		wmedcards[i] = newwin(height, width, ypos + 2 * i, xpos + 2 * i);
		subs[i] = derwin(wmedcards[i], height - 3, width - 8, 3, 4);
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

struct win_pan *patient_info(int regid)
{
	sqlite3_stmt *stmt;
	WINDOW *patient_win, *patient_sub;

	PANEL *patient_pan;
	patient_win = newwin(25, 70, 4, 4);
	patient_sub = derwin(patient_win, 23, 60, 1, 4);
	patient_pan = new_panel(patient_win);
	sqlite3_prepare_v2(db, PATIENT_INFO_REQUEST, strlen(PATIENT_INFO_REQUEST), &stmt, NULL);
	sqlite3_bind_int(stmt, 1, regid);
	sqlite3_step(stmt);

	int rows, i;
	rows = i = 0;
	for (int j = sqlite3_column_count(stmt) - 1; i < j; ++i) {
		if (strlen(sqlite3_column_text(stmt, i)) > 55)
			mvwprintw(patient_sub, 2 + rows++, 0, "%s: %s",
					sqlite3_column_name(stmt, i), sqlite3_column_text(stmt, i));
		else
			mvwprintw(patient_sub, 2 + rows, 0, "%s: %s",
					sqlite3_column_name(stmt, i), sqlite3_column_text(stmt, i));
		++rows;
	}
	sqlite3_finalize(stmt);	
				
	box(patient_win, 0,0);
	struct win_pan *resp = malloc(sizeof(struct win_pan));
	resp->win = patient_win;
	resp->pan = patient_pan;
	resp->sub = patient_sub;
	return resp;
}

char **get_input(char *msg, char **desc, int count, int height, int width, char **regexes, char **defaults, int *heights) //intro message, field descriptions, field count, field height, field width
{
	int x, y, d = 0, c = 0;
	char buffer[count][10000], **retval;
	WINDOW *text_win, *sub;
	FORM *text_form;
	FIELD *fields[count + 2];
	PANEL *text_pan;

	if (desc != NULL)
		while (c < count) {
			d = strlen(desc[c]) > d ? strlen(desc[c]) : d; 
			++c;
		}
	d = d / 2 + 4;
	int last_y = 2;
	if (heights)
		for (int i = 0; i < count; ++i) {
			fields[i] = new_field(heights[i], width, last_y, 1, 0, 0);	
			set_field_opts(fields[i], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
			last_y += heights[i] + 2;
		}
	else
		for (int i = 0; i < count; ++i) {
			fields[i] = new_field(height, width, 2 + i * (height + 2), 1, 0, 0);	
			set_field_opts(fields[i], ~(O_AUTOSKIP | O_BLANK | O_STATIC));
		}

	if (heights)
		fields[count] = new_field(1, 5, last_y, (width + d) / 2 - d, 0, 0);
	else
		fields[count] = new_field(1, 5, 2 + count * (height + 2), (width + d) / 2 - d, 0, 0);
	set_field_buffer(fields[count], 0, "Ввод");
	set_field_fore(fields[count], COLOR_PAIR(BLUE));
	set_field_opts(fields[count], ~(O_AUTOSKIP | O_BLANK | O_EDIT));
	if (regexes)
		for (int i = 0; i < count; ++i) {
			set_field_type(fields[i], TYPE_REGEXP, regexes[i]);
		}
	fields[count + 1] = NULL;
	text_form = new_form(fields);
	form_opts_off(text_form, O_BS_OVERLOAD);
	if (defaults)
		for (int i = 0; i < count; ++i) {
			set_field_buffer(fields[i], 0, defaults[i]);
		}

	scale_form(text_form, &y, &x);	
	text_win = newwin(y += 6, (x += 5) + d , (LINES - y - 6) / 2, (COLS - x - d - 5) / 2);
	set_form_win(text_form, text_win);
	set_form_sub(text_form, sub = derwin(text_win, y - 6, x - 5, 3, 1 + d));
	text_pan = new_panel(text_win);
	post_form(text_form);
	refresh();
	update_panels();
	doupdate();
	last_y = 5;

	if (desc != NULL) {
		if (heights) {
			for (int i = 0; i < count; ++i) {
				mvwprintw(text_win, last_y, 2, desc[i]);
				last_y += heights[i] + 2;
			}
			last_y = 4;
			for (int i = 0; i < count; ++i) {
				frame_field(text_win,
						last_y, d,
						last_y + 1 + heights[i], d + width + 2);
				last_y += heights[i] + 2;
			}
			frame_field(text_win,
					last_y, (width + d) / 2 - 1,
					last_y + 2, (width + d) / 2 + 6);
		}
		else {
			for (int i = 0; i < count; ++i) {
				mvwprintw(text_win, 5 + i * (height + 2), 2, desc[i]);
			}
			for (int i = 0; i < count; ++i) {
				frame_field(text_win,
						4 + i * (height + 2), d,
						5 + i * (height + 2) + height, d + width + 2);
			}
			frame_field(text_win,
					4 + count * (height + 2), (width + d) / 2 - 1,
					6 + count * (height + 2), (width + d) / 2 + 6);
		}
	}

	mvwprintw(text_win, 1, 2, msg);
	box(text_win, 0, 0);

	mvwaddch(text_win, 2, 0, ACS_LTEE);
	mvwhline(text_win, 2, 1, ACS_HLINE, x + d - 2);
	mvwaddch(text_win, 2, x + d - 1, ACS_RTEE);

	keypad(text_win, TRUE);
	wrefresh(text_win);
	x = 0; y = 0;

	curs_set(1);
	while ((c = wgetch(text_win)) != KEY_F(2)) {
		switch(c) {
			case 9:
				form_driver(text_form, REQ_NEXT_FIELD);
				form_driver(text_form, REQ_END_LINE);
				if (current_field(text_form) == fields[count]) 
					set_field_fore(fields[count], COLOR_PAIR(WHITE));
				else
					set_field_fore(fields[count], COLOR_PAIR(BLUE));
				break;
			case KEY_DOWN:
				if (current_field(text_form) == fields[count]) {
					form_driver(text_form, REQ_NEXT_FIELD);
					form_driver(text_form, REQ_END_LINE);
				}
				else {
					form_driver(text_form, REQ_NEXT_LINE);
				}
				break;
			case KEY_UP:
				if (current_field(text_form) == fields[count]) {
					form_driver(text_form, REQ_PREV_FIELD);
					form_driver(text_form, REQ_END_LINE);
				}
				else {
					form_driver(text_form, REQ_PREV_LINE);
				}
				break;
			case KEY_LEFT:
				form_driver(text_form, REQ_PREV_CHAR);
				break;
			case KEY_RIGHT:
				form_driver(text_form, REQ_NEXT_CHAR);
				break;
			case KEY_DC:
			case 127:
				form_driver(text_form, REQ_DEL_PREV);
				break;
			case 10:
				if (current_field(text_form) == fields[count])
					goto CLEANUP;
				else {
					form_driver(text_form, REQ_NEXT_LINE);
					form_driver(text_form, REQ_BEG_LINE);
				}
			default:
				form_driver(text_form, c);
		}
	}
CLEANUP:
	retval = malloc(count * sizeof(char *));	
	for (int i = 0; i < count; ++i) {
		retval[i] = calloc(strlen(field_buffer(fields[i], 0)) + 1, 1);
		strcpy(retval[i], field_buffer(fields[i], 0));
		strip(retval[i]);
		trim(retval[i]);
		free_field(fields[i]);
	}

	unpost_form(text_form);
	free_form(text_form);
	werase(text_win);
	wrefresh(text_win);
	del_panel(text_pan);
	delwin(sub);
	delwin(text_win);
	update_panels();
	doupdate();
	return retval;
}

int trim (char *array)
{
	int i = 0, j = 0, space = 0, s = 0;
	char *temp = calloc(strlen(array), 1);
	while (array[i]) {
		if (array[i] == ' ') {
			space = i;
			temp[j++] = ' ';
			while (array[++space] == ' ') ++s;
			if (s)
				temp[j++] = '\n';
			s = 0;
			temp[j++] = array[space++];
			i = space;
		}
		else {
			temp[j] = array[i];
			++j; ++i;
		}
	}
	strcpy(array, temp);
	curs_set(0);
	return 0;
}

int frame_field(WINDOW *win, int ulcy, int ulcx, int lrcy, int lrcx)
{
	mvwaddch(win, ulcy, ulcx, ACS_ULCORNER);
	mvwaddch(win, ulcy, lrcx, ACS_URCORNER);
	mvwaddch(win, lrcy, ulcx, ACS_LLCORNER);
	mvwaddch(win, lrcy, lrcx, ACS_LRCORNER);
	mvwhline(win, ulcy, ulcx + 1, ACS_HLINE, lrcx - ulcx - 1);
	mvwhline(win, lrcy, ulcx + 1, ACS_HLINE, lrcx - ulcx - 1);
	mvwvline(win, ulcy + 1, ulcx, ACS_VLINE, lrcy - ulcy - 1);
	mvwvline(win, ulcy + 1, lrcx, ACS_VLINE, lrcy - ulcy - 1);
	return 0;
}

