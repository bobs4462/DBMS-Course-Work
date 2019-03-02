#!/bin/bash

rm -f clinic.db
sqlite3 clinic.db < /database/code/ddl.sql
sqlite3 clinic.db < /database/code/dml.sql
sqlite3 clinic.db < /database/code/views.sql
