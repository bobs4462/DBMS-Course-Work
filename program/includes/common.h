#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <form.h>
#include <menu.h>
#include <panel.h>
#include <time.h>
extern sqlite3 *db;
enum colors {GREEN = 1, RED, CYAN, MAGENTA, BLUE, WHITE};

int message_box(const char *text, char *header, int y, int x, int h, int w, int buttons);

int show_menu(char **items, int sz, char *msg, int yc, int xc);

char *search(char *header);
int medical_cards(int regid, int EXTRACT); //work with patient's medical cards

void show_card(int cardid);

void bind_windows(PANEL **pmedcards, WINDOW **wmedcards, WINDOW **subs, int cards); 
int frame_field(WINDOW *win, int ulcy, int ulcx, int lrcy, int lrcx);
void strip(char *text);
int show_appointments(int regid);

int get_card_amount(int regid);
void card_populate(PANEL **pmedcards, WINDOW **wmedcards, WINDOW **subs, int regid);
struct win_pan *patient_info(int regid);
char **get_input(char *msg, char **desc, int count, int height, int width, char **regexes, char **defaults, MENU *menu); //intro message, field descriptions, field count, field height, field width
int inside (char (*array)[50], char *value, int size);
void appointment(int regid);//function for appointment creation
int timetable(int mode); //doctor's timetable view
int patient_search(void);

#define MEDCARD_REQUEST "SELECT * FROM medicalcard where card GLOB '*00' || ?"
#define CARD_AMOUNT_REQUEST "SELECT count(*) FROM medcard where regid = ?"
#define REQORD_AMOUNT_REQUEST "SELECT count(*) FROM medicalcard where card GLOb '*00' || ?"
#define PATIENT_INFO_REQUEST "SELECT * from patient_info where regid = ?"

struct win_pan {
	WINDOW *win;
	WINDOW *sub;
	PANEL *pan;
};
