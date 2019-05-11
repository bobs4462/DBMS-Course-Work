CREATE TRIGGER insert_analysis AFTER INSERT ON analysis 
BEGIN 
	INSERT INTO appointment_history SELECT * FROM appointment
	WHERE regid = new.regid AND date(recdatetime) < new.passdate;
END;

CREATE TRIGGER insert_treatment AFTER INSERT ON treatment 
BEGIN 
	INSERT INTO appointment_history SELECT * FROM appointment
	WHERE regid = new.regid AND date(recdatetime) < new.passdate;
END;

CREATE TRIGGER insert_visit AFTER INSERT ON visit
BEGIN 
	INSERT INTO appointment_history SELECT * FROM appointment
	WHERE regid = new.regid AND date(recdatetime) < new.passdate;
END;
