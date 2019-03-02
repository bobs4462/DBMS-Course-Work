#!/bin/bash

rm -f ./database/clinic.db ./database/code/ddl.sql ./database/code/dml.sql
cat ./database/code/*ddl* > ./database/code/ddl.sql
cat ./database/code/*dml* > ./database/code/dml.sql
sqlite3 ./database/clinic.db < ./database/code/ddl.sql
sqlite3 ./database/clinic.db < ./database/code/dml.sql
sqlite3 ./database/clinic.db < ./database/code/views.sql
