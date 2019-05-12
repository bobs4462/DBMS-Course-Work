#include <common.h>
int card_create(int regid);
int medcard_search(void);
int create_patient(void);
int alter_patient(void);
int delete_patient(void);
int patient_archive(void);
int medical_cards_history(int regid); //work with patient's medical cards
void card_populate_hist(PANEL **pmedcards, WINDOW **wmedcards, WINDOW **subs, int regid);
void show_card_hist(int cardid);
int get_card_amount_hist(int regid);
struct win_pan *patient_info_hist(int regid);
