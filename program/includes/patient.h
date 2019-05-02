#include <gui.h>

#define MEDCARD_REQUEST "SELECT * FROM medicalcard where card GLOB '*00' || ?"
#define CARD_AMOUNT_REQUEST "SELECT count(*) FROM medcard where regid = ?"
#define REQORD_AMOUNT_REQUEST "SELECT count(*) FROM medicalcard where card GLOb '*00' || ?"
#define DOCTOR_LIST_REQUEST "SELECT fio, position, tabid from employee where position <> 'регистратура'"
#define PATIENT_INFO_REQUEST "SELECT * from patient_info where regid = ?"
#define PATIENT_PASS_UPDATE "UPDATE patient set password = ? where regid = ?"

typedef struct menu_struct {
	char **dlist; //doctors list
	char **plist; //position list
	ITEM **doctors; //menu items for subsequent cleanup
} * DMS; //doctor menu structure;

void appointment(int regid);
int timetable(int mode);
void medical_cards(int regid);
void init_menu(ITEM ***some_items, char **some_choices, size_t n_choices);
int get_card_amount(int regid);
void bind_windows(PANEL **pmedcards, WINDOW **wmedcards, int cards);
void card_populate(PANEL **pmedcards, WINDOW **wmedcards, int regid);
void show_card(int cardid);
MENU *doctor_list(DMS *clup);
void free_ms(DMS clup);
void password_change(int regid); 
void print_timetable(int regid);
int show_menu(char **items, int sz, char *msg);
void update_pass(int regid, char *new_pass);
