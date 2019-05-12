CREATE TABLE sickleave (
    sickid integer NOT NULL PRIMARY KEY,
	sick_code integer NOT NULL,
	tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid) ON DELETE CASCADE,
    issuedate date NOT NULL DEFAULT (date('now')),
    stdate date NOT NULL,
    endate date NOT NULL,
    destn text NOT NULL
);
