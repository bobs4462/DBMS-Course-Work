CREATE TABLE sickleave (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    issuedate date NOT NULL DEFAULT (date('now')),
    stdate date NOT NULL,
    endate date NOT NULL,
    destn text NOT NULL,
    PRIMARY KEY(tabid, regid, issuedate)
);
