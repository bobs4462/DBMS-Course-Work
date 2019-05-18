CREATE TABLE medcard (
    cardid integer NOT NULL PRIMARY KEY,
    regid integer NOT NULL REFERENCES patient(regid),
    crdate date NOT NULL DEFAULT (date('now')),
    type text,
);
