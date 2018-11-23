CREATE TABLE patchat (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    crtime datetime NOT NULL DEFAULT (datetime('now')),
    lhist text,
    PRIMARY KEY(tabid, regid)
);
