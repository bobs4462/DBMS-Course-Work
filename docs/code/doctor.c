void doctor_interface(int tabid)
{
	int regid, repeat = 0;
SEARCH:
	regid = patient_search();
	struct win_pan *pat = patient_info(regid);
	char *options[] = {
		"Просмотр карт пациента",
		"Назначение анализов",
		"Внести результат анализов",
		"Назначение лечения",
		"Выписка рецепта",
		"Выдача листка нетрудоспособности",
		"Просмотр листов нетрудоспособности",
		"Повторить поиск",
		"Выход",
		NULL
	};
	while(TRUE) {
		switch (show_menu(
					options,
					10,
					"Выберите действие",
					4,
					COLS - 70)) {
			case 0:
				medical_cards(regid, 0);
				break;
			case 1:
				annalysis_create(tabid, regid);
				break;
			case 2:
				annalysis_update(regid);
				break;
			case 3:
				cure_create(regid, tabid);
				break;
			case 4:
				receipt_issue(tabid, regid);
				break;
			case 5:
				sick_leave_issue(regid, tabid);
				break;
			case 6:
				sickleave_list(tabid, regid);
				break;
			case 7:
				repeat = 1;
				goto EXIT;
				break;
			default:
				repeat = 0;
				goto EXIT;
		}
	}

EXIT:
	del_panel(pat->pan);
	delwin(pat->sub);
	delwin(pat->win);
	update_panels();
	doupdate();
	if (repeat) {
		repeat = 0;
		goto SEARCH;
	}
	return;
}
