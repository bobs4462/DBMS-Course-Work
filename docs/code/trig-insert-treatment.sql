CREATE TRIGGER insert_treatment AFTER INSERT ON treatment 
BEGIN 
	INSERT INTO appointment_history SELECT * FROM appointment
	WHERE regid = (SELECT  regid FROM medcard WHERE cardid = new.cardid)
	AND date(recdatetime) <= new.trdate;
	DELETE FROM appointment 
	WHERE regid = (SELECT  regid FROM medcard WHERE cardid = new.cardid) AND date(recdatetime) <= new.trdate
	AND tabid = new.tabid;
END;
