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
