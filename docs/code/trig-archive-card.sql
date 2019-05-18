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
