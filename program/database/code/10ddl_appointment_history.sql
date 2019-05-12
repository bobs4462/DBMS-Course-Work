CREATE TABLE appointment_history (
    tabid integer NOT NULL REFERENCES employee(tabid),
    regid integer NOT NULL REFERENCES patient(regid) ON DELETE CASCADE,
    recdatetime datetime
);
