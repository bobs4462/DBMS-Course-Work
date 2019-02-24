#include <gui.h>
#define PASS_REQUEST "SELECT * FROM pat_pass where regid = ? AND password = ?"

int get_regid(char *login);
int pass_verify(char *, char *);
int authenticate(int regid, char *pass);
