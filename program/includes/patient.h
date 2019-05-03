#include <gui.h>

#define DOCTOR_LIST_REQUEST "SELECT fio, position, tabid from employee where position <> 'регистратура'"
#define PATIENT_PASS_UPDATE "UPDATE patient set password = ? where regid = ?"

typedef struct menu_struct {
	char **dlist; //doctors list
	char **plist; //position list
	ITEM **doctors; //menu items for subsequent cleanup
} * DMS; //doctor menu structure;

void appointment(int regid);
int timetable(int mode);
void init_menu(ITEM ***some_items, char **some_choices, size_t n_choices);
int get_card_amount(int regid);
MENU *doctor_list(DMS *clup);
void free_ms(DMS clup);
void password_change(int regid); 
void print_timetable(int regid);
void update_pass(int regid, char *new_pass);
