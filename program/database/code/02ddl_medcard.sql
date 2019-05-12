CREATE TABLE medcard (
    cardid integer NOT NULL PRIMARY KEY,
    regid integer NOT NULL REFERENCES patient(regid)
	ON DELETE RESTRICT,
    crdate date NOT NULL DEFAULT (date('now')),
    type text,
    UNIQUE(regid, type)
);
CREATE TABLE medcard_history (
    cardid integer NOT NULL PRIMARY KEY,
    regid integer NOT NULL,
    crdate date NOT NULL DEFAULT (date('now')),
    type text,
    UNIQUE(regid, type)
);
