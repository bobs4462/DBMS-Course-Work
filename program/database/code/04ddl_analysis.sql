CREATE TABLE analysis (
    anid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL REFERENCES employee(tabid),
    cardid  integer NOT NULL REFERENCES medcard(cardid),
    passdate date NOT NULL DEFAULT (date('now')),
    type text NOT NULL,
    result text
);
CREATE TABLE analysis_history (
    anid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL,
    cardid  integer NOT NULL,
    passdate date NOT NULL DEFAULT (date('now')),
    type text NOT NULL,
    result text
);
