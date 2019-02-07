enum user_types {PATIENT, DOCTOR, REGISTRY, UNKNOWN}; 
typedef enum user_types user_t;
/*INCLUDES*/
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#ifdef MAIN_H

/*FUNCTION PROTOTYPES*/
#include <locale.h>
user_t login(char **);
void patient_interface(sqlite3 *db);
void doctor_interface(sqlite3 *db);
void registry_interface(sqlite3 *db);

#define LOG_INCORRECT "Ошибка! Ввдеден неправильный логин"

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
#endif

