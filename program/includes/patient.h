#include <gui.h>

#define MEDCARD_REQUEST "SELECT * FROM medicalcard where card GLOB '*0' || ?"
#define CARD_AMOUNT_REQUEST "SELECT count(*) FROM medcard where regid = ?"

void appointement(int regid);
void timetable(int regid);
void medical_cards(int regid);
void init_menu(ITEM ***some_items, char **some_choices, size_t n_choices);
int get_card_amount(int regid);
void bind_windows(PANEL **pmedcards, WINDOW **wmedcards, int cards);
void card_populate(PANEL **pmedcards, WINDOW **wmedcards, int regid);
void show_card(int cardid);
