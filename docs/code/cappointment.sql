CREATE TABLE appointment (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    recdatetime datetime,
    PRIMARY KEY(tabid, regid),
);
