#include <sqlite3.h>
#ifdef SQL_ADAPTER_H
extern sqlite3 *db;
#define PASS_REQUEST "SELECT * FROM pat_pass where regid = ? AND password = ?"
#define MEDCARD_REQUEST "SELECT * FROM medicalcard where card GLOB '*0' || ?"
#define CARD_AMOUNT_REQUEST "SELECT count(*) FROM medcard where regid = ?"
#include <string.h>
#endif

int authenticate(int, char *);

