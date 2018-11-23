CREATE TABLE reservation (
    servid integer NOT NULL REFERENCES service(servid),
    regid integer NOT NULL REFERENCES patient(regid),
    rdate date NOT NULL DEFAULT (date('now')),
    pdate datetime NOT NULL,
    status text NOT NULL,
    PRIMARY KEY(servid, regid)
);
