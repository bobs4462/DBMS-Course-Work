enum user_types {PATIENT, DOCTOR, REGISTRY}; 
typedef enum user_types user_t;
/*INCLUDES*/
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#define LOG_INCORRECT "Ошибка! Ввeдён неправильный логин или пароль!"

#ifdef MAIN_H

/*FUNCTION PROTOTYPES*/
#include <locale.h>
user_t login(int regid);
void patient_interface(int regid);
void doctor_interface(int regid);
void registry_interface(int regid);


#endif

#ifdef GUI_H
#include <sqlite3.h>
#include <form.h>
#include <menu.h>
#include <panel.h>

/*MACRO DEFINITIONS*/
#define LOG_WIDTH 35
#define LOG_HIGHT 3


enum colors {GREEN = 1, RED, CYAN, MAGENTA, BLUE};

/*FUNCTION PROTOTYPES*/
void init_menu(ITEM ***some_items, char **some_choices, size_t n_choices);
int get_regid(char *login);
int pass_verify(char *, char *);
void appointement(void);
void timetable(void);
void medical_cards(void);
#endif


extern sqlite3 *db;
