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
	pnumber AS 'Контактный телефон',
	regid
FROM patient;
