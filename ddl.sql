CREATE TABLE employee (
    tabid integer PRIMARY KEY,
    fio text NOT NULL,
    position text NOT NULL,
    experien integer NOT NULL DEFAULT 0,
    birthdate date NOT NULL,
    gender char(1) NOT NULL CHECK(gender LIKE 'М' 
        OR gender LIKE 'Ж'),
    address text,
    pnumber text,
    password text NOT NULL DEFAULT (abs(random()))
);
CREATE TABLE patient (
    regid integer PRIMARY KEY,
    fio text NOT NULL,
    birthdate date NOT NULL,
    gender char(1) NOT NULL CHECK(gender LIKE 'М' 
        OR gender LIKE 'Ж'),
    address text,
    pnumber text,
    password text NOT NULL DEFAULT (abs(random()))
);
CREATE TABLE empchat (
    stabid integer not null REFERENCES employee(tabid),
    dtabid integer not null REFERENCES employee(tabid),
    crtime datetime not null DEFAULT (datetime('now')),
    lhist blob,
    PRIMARY KEY(stabid, dtabid)
);
CREATE TABLE medcard (
    cardid integer not null PRIMARY KEY,
    regid integer not null REFERENCES patient(regid),
    crdate date not null DEFAULT (date('now')),
    type text,
    UNIQUE(regid, type)
);
CREATE TABLE patchat (
    tabid integer not null REFERENCES employee(tabid),
    regid integer not null REFERENCES patient(regid),
    crtime datetime not null DEFAULT (datetime('now')),
    lhist blob,
    PRIMARY KEY(tabid, regid)
);
CREATE TABLE appointment (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    recdatetime datetime,
    PRIMARY KEY(tabid, regid),
    UNIQUE(tabid, recdatetime),
    UNIQUE(regid, recdatetime)
);
CREATE TABLE resultan (
    resid integer not null PRIMARY KEY,
    cardid  integer not null REFERENCES medcard(cardid),
    type text not null,
    passdate date not null DEFAULT (date('now')),
    result text
);
CREATE TABLE service (
    servid integer not null PRIMARY KEY,
    name text not null unique,
    descrip text not null,
    cost integer not null DEFAULT 0
);

CREATE TABLE reservation (
    servid integer not null REFERENCES service(servid),
    regid integer not null REFERENCES patient(regid),
    rdate date not null DEFAULT (date('now')),
    pdate datetime not null,
    PRIMARY KEY(servid, regid)
);
CREATE TABLE treatment (
    tabid integer not null REFERENCES employee(tabid),
    cardid integer not null REFERENCES medcard(cardid),
    trdate date not null DEFAULT (date('now')),
    illness text,
    treatment text,
    PRIMARY KEY(tabid, cardid)
);
