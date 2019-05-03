#include <gui.h>
#define PASS_REQUEST "SELECT * FROM patient where regid = ? AND password = ?"
#define PASS_REQUEST_D "SELECT * FROM employee where tabid = ? AND password = ?"
#define HELLO

int get_regid(char *login);
int pass_verify(char *, char *, char *);
int authenticate(int regid, char *pass, char *);
