CREATE TRIGGER archive_patient BEFORE DELETE ON patient
BEGIN
	INSERT INTO medcard_history SELECT * FROM medcard
	WHERE regid = old.regid;
	DELETE FROM medcard WHERE regid = old.regid;
	INSERT INTO patient_history SELECT * FROM patient 
	WHERE regid = old.regid;
	DELETE FROM patient WHERE regid = old.regid;
END;
