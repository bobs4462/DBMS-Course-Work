CREATE TABLE receipt (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    issuedate date NOT NULL DEFAULT (date('now')),
    medicine text,
    PRIMARY KEY(tabid, regid, issuedate)
);
