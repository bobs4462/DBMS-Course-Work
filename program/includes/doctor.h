#include <gui.h>
int annalysis_create(int tabid, int regid);
int annalysis_update(int regid);
int cure_create(int regid, int tabid);
int receipt_issue(int tabid, int regid);
int sick_leave_issue(int regid, int tabid);
int show_sickleave_form(sqlite3_stmt *stmt);
int sickleave_list(int tabid, int regid);
