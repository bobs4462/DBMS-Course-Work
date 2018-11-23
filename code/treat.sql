CREATE TABLE treatment (
    tabid integer NOT NULL REFERENCES employee(tabid),
    cardid integer NOT NULL REFERENCES medcard(cardid),
    trdate date NOT NULL DEFAULT (date('now')),
    illness text,
    treatment text,
    PRIMARY KEY(tabid, cardid)
);
