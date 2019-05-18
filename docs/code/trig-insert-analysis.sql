CREATE TRIGGER insert_analysis AFTER INSERT ON analysis 
BEGIN 
	INSERT INTO appointment_history SELECT * FROM appointment
	WHERE regid = 
	(SELECT  regid FROM medcard WHERE cardid = new.cardid)
		AND date(recdatetime) <= new.passdate;
	DELETE FROM appointment 
	WHERE regid = (SELECT  regid FROM medcard WHERE cardid = new.cardid)
	AND date(recdatetime) <= new.passdate
	AND tabid = new.tabid;
END;
