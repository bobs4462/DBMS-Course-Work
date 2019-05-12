CREATE TABLE visit (
    visid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL REFERENCES employee(tabid),
    cardid integer NOT NULL REFERENCES medcard(cardid),
    visdate date NOT NULL DEFAULT (date('now')),
    visgoal text
);
CREATE TABLE visit_history (
    visid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL,
    cardid integer NOT NULL,
    visdate date NOT NULL DEFAULT (date('now')),
    visgoal text
);
