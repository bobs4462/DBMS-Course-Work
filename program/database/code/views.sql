CREATE VIEW medicalcard AS 
SELECT 
	"AN00000" || an.anid, 
	"D000000" || an.tabid,
	emp.fio,
	emp.position,
	"C000000" || an.cardid as card,
	an.passdate AS occurence,
	an.type,
	an.result
	FROM analysis an INNER JOIN employee emp ON emp.tabid = an.tabid 
UNION ALL 
SELECT 
	"TR00000" || tr.treatid, 
	"D000000" || tr.tabid,
	emp.fio,
	emp.position,
	"C000000" || tr.cardid AS card,
	tr.trdate AS occurence,
	tr.illness,
	tr.treatment
	FROM treatment tr INNER JOIN employee emp ON emp.tabid = tr.tabid
UNION ALL
SELECT 
	"VS00000" || vis.visid,
	"D000000" || vis.tabid,
	emp.fio,
	emp.position,
	"C000000" || vis.cardid AS card,
	vis.visdate AS occurence,
	vis.visgoal,
	' '
	FROM visit vis INNER JOIN employee emp ON emp.tabid = vis.tabid
ORDER BY occurence;


	
