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
