CREATE TABLE timetable (
    tabid integer NOT NULL REFERENCES employee(tabid),
	daynum integer NOT NULL,
    weekday text NOT NULL,
    shiftst integer NOT NULL,
    shiftend integer NOT NULL,
    break integer NOT NULL,
    PRIMARY KEY(tabid, weekday)
);
