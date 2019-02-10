enum user_types {PATIENT, DOCTOR, REGISTRY, UNKNOWN}; 
typedef enum user_types user_t;
/*INCLUDES*/
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define LOG_INCORRECT "Ошибка! Ввeдён неправильный логин или пароль!"

#ifdef MAIN_H

/*FUNCTION PROTOTYPES*/
#include <locale.h>
user_t login(void);
void patient_interface(void);
void doctor_interface(void);
void registry_interface(void);


#endif

#ifdef GUI_H
#include <sqlite3.h>
#include <form.h>
#include <menu.h>
#include <panel.h>

/*MACRO DEFINITIONS*/
#define LOG_WIDTH 35
#define LOG_HIGHT 3

/*FUNCTION PROTOTYPES*/
void init_menu(ITEM ***some_items, char **some_choices, size_t n_choices);
int pass_verify(char *, char *);
void appointement(void);
void timetable(void);
void medical_cards(void);
#endif


extern sqlite3 *db;
