SELECT * FROM patient where regid = ? AND password = ?
SELECT * FROM employee where tabid = ? AND password = ?
SELECT * from patient_info where regid = ?
SELECT vacant_time(?, ?)
	INSERT INTO appointment values(?, ?, ?)
SELECT e.fio, a.recdatetime, a.tabid FROM appointment a 
	INNER JOIN employee e ON a.tabid = e.tabid 
	WHERE (a.regid = ? AND a.tabid = ?) OR (a.regid = ? AND a.recdatetime = ?)
SELECT a.recdatetime, e.fio, e.position FROM appointment a
   	JOIN employee e on a.tabid = e.tabid WHERE regid = ? 
SELECT r.receiptid, r.medicine, r.issuedate, e.fio, p.fio, 
	CAST ((julianday('now') - julianday(p.birthdate)) / 365.25 as INTEGER), 
	p.address FROM 
	receipt r JOIN employee e ON e.tabid = r.tabid JOIN 
	patient p ON p.regid = r.regid where r.regid = ?
SELECT weekday, get_time(shiftst), get_time(shiftend), get_time(break)
    FROM timetable where tabid = ? and shiftst IS NOT NULL ORDER BY daynum
SELECT * FROM patient_medcard where regid = ?
SELECT count(*) FROM medcard where regid = ?
SELECT count(*) FROM medicalcard where card GLOB '*00' || ?
SELECT * FROM medicalcard where card GLOB '*00' || ?
UPDATE patient set password = ? where regid = ?
