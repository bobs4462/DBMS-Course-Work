CREATE TABLE resultan (
    resid integer NOT NULL PRIMARY KEY,
    cardid  integer NOT NULL REFERENCES medcard(cardid),
    type text NOT NULL,
    passdate date NOT NULL DEFAULT (date('now')),
    result text
);
