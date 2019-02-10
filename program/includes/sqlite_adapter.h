#include <sqlite3.h>
#ifdef SQL_ADAPTER_H
extern sqlite3 *db;
#define PASS_REQUEST "SELECT * FROM pat_pass where regid = ? AND password = ?"
#include <string.h>
#endif

int authenticate(int, char *);
