CREATE VIEW patient_medcard AS 
SELECT 
	pat.regid AS regid,
	mc.cardid,
	pat.fio,
	mc.crdate AS occurence,
	mc.type,
	(SELECT count(*) FROM medicalcard WHERE card LIKE "%00" || mc.cardid) AS rec_count	
	FROM medcard mc INNER JOIN patient pat ON pat.regid = mc.regid
ORDER BY occurence;
