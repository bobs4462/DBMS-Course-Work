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

CREATE TRIGGER insert_treatment AFTER INSERT ON treatment 
BEGIN 
	INSERT INTO appointment_history SELECT * FROM appointment
	WHERE regid = (SELECT  regid FROM medcard WHERE cardid = new.cardid)
	AND date(recdatetime) <= new.trdate;
	DELETE FROM appointment 
	WHERE regid = (SELECT  regid FROM medcard WHERE cardid = new.cardid) AND date(recdatetime) <= new.trdate
	AND tabid = new.tabid;
END;

CREATE TRIGGER insert_visit AFTER INSERT ON visit
BEGIN 
	INSERT INTO appointment_history SELECT * FROM appointment
	WHERE regid = (SELECT  regid FROM medcard WHERE cardid = new.cardid) AND date(recdatetime) <= new.visdate;
	DELETE FROM appointment 
	WHERE regid = (SELECT  regid FROM medcard WHERE cardid = new.cardid) AND date(recdatetime) <= new.visdate
	AND tabid = new.tabid;
END;


CREATE TRIGGER archive_card BEFORE DELETE ON medcard
BEGIN 
	INSERT INTO analysis_history SELECT * FROM analysis
	WHERE cardid = old.cardid;
	DELETE FROM analysis WHERE cardid = old.cardid;

	INSERT INTO treatment_history SELECT * FROM treatment
	WHERE cardid = old.cardid;
	DELETE FROM treatment WHERE cardid = old.cardid;

	INSERT INTO visit_history SELECT * FROM visit
	WHERE cardid = old.cardid;
	DELETE FROM visit WHERE cardid = old.cardid;

	DELETE FROM medcard WHERE cardid = old.cardid;
END;

CREATE TRIGGER archive_patient BEFORE DELETE ON patient
BEGIN
	INSERT INTO medcard_history SELECT * FROM medcard
	WHERE regid = old.regid;
	DELETE FROM medcard WHERE regid = old.regid;
	INSERT INTO patient_history SELECT * FROM patient 
	WHERE regid = old.regid;
	DELETE FROM patient WHERE regid = old.regid;
END;
