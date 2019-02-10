#define SQL_ADAPTER_H
#include <stdio.h>
#include <sqlite_adapter.h>

int authenticate(int regid, char *pass)
{
	int rv;
	sqlite3_stmt *stmt;
	const char *tail;
	sqlite3_prepare_v2(db, PASS_REQUEST, strlen(PASS_REQUEST), &stmt, &tail);
	sqlite3_bind_int(stmt, 1, regid);
	sqlite3_bind_text(stmt, 2, pass, strlen(pass), SQLITE_TRANSIENT);
	if (sqlite3_step(stmt) == SQLITE_ROW)
		rv = 0;
	else {
		fprintf(stderr, "ERROR: %s", sqlite3_errmsg(db));
		rv = 1;
	}
	sqlite3_finalize(stmt);
	return rv;
}


