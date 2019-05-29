SELECT regid, fio FROM patient WHERE fio LIKE ?
INSERT INTO analysis(tabid, cardid, type) values(?, ?, ?)
SELECT last_insert_rowid()
SELECT anid, type FROM analysis WHERE cardid IN (SELECT cardid FROM
	medcard WHERE regid = ?) AND result IS NULL
UPDATE analysis SET result = ? WHERE anid = ?
INSERT INTO treatment(tabid, cardid, illness, treatment)
	VALUES(?, ?, ?, ?)
SELECT last_insert_rowid()
INSERT INTO receipt(tabid, regid, medicine)
    VALUES(?, ?, ?)
INSERT INTO sickleave(regid, tabid, stdate, endate, destn, sick_code)
    VALUES(?, ?, ?, ?, ?, ?)
SELECT s.sickid, strftime('%d-%m-%Y', s.issuedate),
	strftime('%d-%m-%Y', s.stdate), 
	strftime('%d-%m-%Y', s.endate),
	strftime('%d-%m-%Y',date(s.endate, '+1 day')), 
	s.destn, e.fio, e.position, p.fio, p.gender,
	strftime('%d-%m-%Y',p.birthdate), s.sick_code
	FROM sickleave s \
	JOIN employee e ON s.tabid = e.tabid \
	JOIN patient p ON p.regid = s.regid\
	WHERE s.tabid = ? and s.regid = ?
