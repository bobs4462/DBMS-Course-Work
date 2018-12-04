CREATE TABLE analysis (
    anid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL REFERENCES employee(tabid),
    cardid  integer NOT NULL REFERENCES medcard(cardid),
    passdate date NOT NULL DEFAULT (date('now')),
    type text NOT NULL,
    result text
);
CREATE TABLE appointment (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    recdatetime datetime,
    PRIMARY KEY(tabid, regid),
    UNIQUE(tabid, recdatetime),
    UNIQUE(regid, recdatetime)
);
CREATE TABLE employee (
    tabid integer PRIMARY KEY,
    fio text NOT NULL,
    position text NOT NULL,
    experien integer NOT NULL DEFAULT 0,
    birthdate date NOT NULL,
    gender char(1) NOT NULL CHECK(gender = 'М' 
        OR gender = 'Ж'),
    address text,
    pnumber text,
    password text NOT NULL DEFAULT (abs(random()))
);
CREATE TABLE medcard (
    cardid integer NOT NULL PRIMARY KEY,
    regid integer NOT NULL REFERENCES patient(regid),
    crdate date NOT NULL DEFAULT (date('now')),
    type text,
    UNIQUE(regid, type)
);
CREATE TABLE patient (
    regid integer PRIMARY KEY,
    fio text NOT NULL,
    birthdate date NOT NULL,
    gender char(1) NOT NULL CHECK(gender = 'М' 
        OR gender = 'Ж'),
    address text,
    pnumber text,
    password text NOT NULL DEFAULT (abs(random()))
);
CREATE TABLE receipt (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    issuedate date NOT NULL DEFAULT (date('now')),
    medicine text,
    PRIMARY KEY(tabid, regid, issuedate)
);
CREATE TABLE serviceprovision (
    name text NOT NULL REFERENCES service(name),
    cardid integer NOT NULL REFERENCES medcard(cardid),
    provdate date NOT NULL DEFAULT (date('now')),
    comment text,
    PRIMARY KEY(name, cardid, provdate)
);
CREATE TABLE service (
    name text NOT NULL PRIMARY KEY,
    descrip text NOT NULL,
    cost integer NOT NULL DEFAULT 0
);
CREATE TABLE sickleave (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    issuedate date NOT NULL DEFAULT (date('now')),
    stdate date NOT NULL,
    endate date NOT NULL,
    destn text NOT NULL,
    PRIMARY KEY(tabid, regid, issuedate)
);
CREATE TABLE timetable (
    tabid integer NOT NULL REFERENCES employee(tabid),
    weekday text NOT NULL,
    shiftst integer NOT NULL,
    shiftend integer NOT NULL,
    break integer NOT NULL,
    PRIMARY KEY(tabid, weekday)
);
CREATE TABLE treatment (
    tabid integer NOT NULL REFERENCES employee(tabid),
    cardid integer NOT NULL REFERENCES medcard(cardid),
    trdate date NOT NULL DEFAULT (date('now')),
    illness text,
    treatment text,
    PRIMARY KEY(tabid, cardid, trdate)
);
CREATE TABLE visit (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    visdate date NOT NULL DEFAULT (date('now')),
    visgoal text,
    PRIMARY KEY(tabid, regid, visdate)
);
