enum user_types {PATIENT, DOCTOR, REGISTRY, UNKNOWN}; 
typedef enum user_types user_t;
/*INCLUDES*/
#include <ncurses.h>

#ifdef MAIN_H

/*FUNCTION PROTOTYPES*/
#include <locale.h>
user_t login(char **);
void patient_interface(void);

#endif

#ifdef GUI_H
#include <form.h>
#include <menu.h>
#include <panel.h>
#include <stdlib.h>
#include <string.h>

/*MACRO DEFINITIONS*/
#define LOG_WIDTH 35
#define LOG_HIGHT 3

/*FUNCTION PROTOTYPES*/
void init_menu(ITEM ***some_items, char **some_choices, size_t n_choices);
#endif


