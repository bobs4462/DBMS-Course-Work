CREATE TABLE service (
    servid integer NOT NULL PRIMARY KEY,
    name text NOT NULL unique,
    descrip text NOT NULL,
    cost integer NOT NULL DEFAULT 0
);
