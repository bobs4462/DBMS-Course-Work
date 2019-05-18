CREATE TRIGGER insert_visit AFTER INSERT ON visit
BEGIN 
	INSERT INTO appointment_history SELECT * FROM appointment
	WHERE regid = (SELECT  regid FROM medcard WHERE cardid = new.cardid) AND date(recdatetime) <= new.visdate;
	DELETE FROM appointment 
	WHERE regid = (SELECT  regid FROM medcard WHERE cardid = new.cardid) AND date(recdatetime) <= new.visdate
	AND tabid = new.tabid;
END;
