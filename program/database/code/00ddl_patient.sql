CREATE TABLE patient (
    regid integer PRIMARY KEY,
    fio text NOT NULL,
    insurcomp text,
    icontract text,
    birthdate date NOT NULL,
    gender char(1) NOT NULL DEFAULT 'М' CHECK(gender = 'М' OR gender = 'Ж'),
    address text,
    pnumber text,
    password text NOT NULL DEFAULT (abs(random()))
);

CREATE TABLE patient_history (
    regid integer PRIMARY KEY,
    fio text NOT NULL,
    insurcomp text,
    icontract text,
    birthdate date NOT NULL,
    gender char(1) NOT NULL DEFAULT 'М' CHECK(gender = 'М' OR gender = 'Ж'),
    address text,
    pnumber text,
    password text NOT NULL DEFAULT (abs(random()))
);
