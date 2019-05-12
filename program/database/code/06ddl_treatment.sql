CREATE TABLE treatment (
    treatid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL REFERENCES employee(tabid),
    cardid integer NOT NULL REFERENCES medcard(cardid),
    trdate date NOT NULL DEFAULT (date('now')),
    illness text,
    treatment text
);
CREATE TABLE treatment_history (
    treatid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL,
    cardid integer NOT NULL,
    trdate date NOT NULL DEFAULT (date('now')),
    illness text,
    treatment text
);
