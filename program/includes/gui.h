/*INCLUDES*/
#include <common.c>
enum user_types {PATIENT = 63, DOCTOR, REGISTRY, UNKNOWN}; 
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




