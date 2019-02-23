CREATE TABLE receipt (
    receiptid integer NOT NULL PRIMARY KEY,
    tabid integer NOT NULL REFERENCES employee(tabid),
    issuedate date NOT NULL DEFAULT (date('now')),
    medicine text
);
