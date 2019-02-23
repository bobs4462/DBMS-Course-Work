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
