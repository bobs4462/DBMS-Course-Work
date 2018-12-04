CREATE TABLE visit (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    visdate date NOT NULL DEFAULT (date('now')),
    visgoal text,
    PRIMARY KEY(tabid, regid, visdate)
);
