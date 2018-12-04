CREATE TABLE serviceprovision (
    name text NOT NULL REFERENCES service(name),
    cardid integer NOT NULL REFERENCES medcard(cardid),
    provdate date NOT NULL DEFAULT (date('now')),
    comment text,
    PRIMARY KEY(name, cardid, provdate)
);
