CREATE TABLE employee (
    tabid integer PRIMARY KEY,
    fio text NOT NULL,
    position text NOT NULL,
    experien integer NOT NULL DEFAULT 0,
    birthdate date NOT NULL,
    gender char(1) NOT NULL CHECK(gender = 'М' OR gender = 'Ж'),
    address text,
    pnumber text,
    password text NOT NULL DEFAULT (abs(random()))
);
CREATE TABLE patient (
    regid integer PRIMARY KEY,
    fio text NOT NULL,
    insurcomp text,
    icontract text,
    birthdate date NOT NULL,
    gender char(1) NOT NULL CHECK(gender = 'М' OR gender = 'Ж'),
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
CREATE TABLE receipt (
    receiptid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid),
    issuedate date NOT NULL DEFAULT (date('now')),
    medicine text
);
CREATE TABLE sickleave (
	sickid integer NOT NULL PRIMARY KEY,
    regid integer NOT NULL REFERENCES patient(regid),
    issuedate date NOT NULL DEFAULT (date('now')),
    stdate date NOT NULL,
    endate date NOT NULL,
    destn text NOT NULL
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
    treatid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL REFERENCES employee(tabid),
    cardid integer NOT NULL REFERENCES medcard(cardid),
    trdate date NOT NULL DEFAULT (date('now')),
    illness text,
    treatment text
);
CREATE TABLE visit (
    visid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL REFERENCES employee(tabid),
    cardid integer NOT NULL REFERENCES medcard(cardid),
    visdate date NOT NULL DEFAULT (date('now')),
    visgoal text
);
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
