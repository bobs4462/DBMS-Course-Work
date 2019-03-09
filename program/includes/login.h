#include <gui.h>
#define PASS_REQUEST "SELECT * FROM patient where regid = ? AND password = ?"
#define HELLO

int get_regid(char *login);
int pass_verify(char *, char *);
int authenticate(int regid, char *pass);
