CREATE VIEW medicalcard AS 
SELECT 
	"AN00000" || an.anid as type,
	"D000000" || an.tabid AS doctor_num,
	emp.fio,
	emp.position,
	"C000000" || an.cardid as card,
	an.passdate AS occurence,
	an.type AS antype,
	an.result
	FROM analysis an INNER JOIN employee emp ON emp.tabid = an.tabid 
UNION ALL 
SELECT 
	"TR00000" || tr.treatid AS type, 
	"D000000" || tr.tabid AS doctor_num,
	emp.fio,
	emp.position,
	"C000000" || tr.cardid AS card,
	tr.trdate AS occurence,
	tr.illness,
	tr.treatment
	FROM treatment tr INNER JOIN employee emp ON emp.tabid = tr.tabid
UNION ALL
SELECT 
	"VS00000" || vis.visid AS type,
	"D000000" || vis.tabid AS doctor_num,
	emp.fio,
	emp.position,
	"C000000" || vis.cardid AS card,
	vis.visdate AS occurence,
	vis.visgoal,
	' '
	FROM visit vis INNER JOIN employee emp ON emp.tabid = vis.tabid
ORDER BY occurence;

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

CREATE VIEW patient_info AS 
SELECT 
	fio AS ФИО,
	insurcomp AS 'Страховая компания',
	icontract AS 'Номер полиса страхования',
	birthdate AS 'Дата рождения',
	CASE gender
	WHEN 'М' THEN 'мужчина'
	WHEN 'Ж' THEN 'женщина'
	END AS 'Пол',
	address AS 'Адрес проживания',
	pnumber AS 'Контактный телефон'
FROM patient;
