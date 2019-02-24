/*INCLUDES*/
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <form.h>
#include <menu.h>
#include <panel.h>

enum user_types {PATIENT, DOCTOR, REGISTRY}; 
typedef enum user_types user_t;

#define LOG_INCORRECT "Ошибка! Ввeдён неправильный логин или пароль!"

#ifdef MAIN_H

/*FUNCTION PROTOTYPES*/
#include <locale.h>
user_t login(int *regid);
void patient_interface(int regid);
void doctor_interface(int regid);
void registry_interface(int regid);


#endif

/*MACRO DEFINITIONS*/

#define LOG_WIDTH 35
#define LOG_HIGHT 3


enum colors {GREEN = 1, RED, CYAN, MAGENTA, BLUE};

extern sqlite3 *db;

