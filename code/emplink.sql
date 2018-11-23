CREATE TABLE empchat (
    stabid integer NOT NULL REFERENCES employee(tabid),
    dtabid integer NOT NULL REFERENCES employee(tabid),
    crtime datetime NOT NULL DEFAULT (datetime('now')),
    lhist text,
    PRIMARY KEY(tabid, regid)
);
