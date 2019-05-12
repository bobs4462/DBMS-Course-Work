CREATE TABLE appointment (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid)
	ON DELETE CASCADE,
    recdatetime datetime,
    PRIMARY KEY(tabid, regid),
    UNIQUE(tabid, recdatetime),
    UNIQUE(regid, recdatetime)
);
