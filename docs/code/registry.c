#include <registry.h>

void registry_interface(void)
{
	char *options[] = {
		"Просмотр карт пациентов",
		"Добавить нового пациента",
		"Изменить данные пациента",
		"Удалить пациента",
		"Просмотр расписания врачей",
		"Создать запись к врачу",
		"Архив пациентов",
		"Выход",
		NULL
	};
	int regid = 0;
	while (TRUE) {
		switch (show_menu(
					options, 
					9, 
					"Доступные действия",
					4,
					-1))
		{
			case 0:
				medcard_search();
				break;
			case 1:
				create_patient();
				break;
			case 2:
				alter_patient();
				break;
			case 3:
				delete_patient();
				break;
			case 4:
				timetable(0);
				break;
			case 5:
				regid = patient_search();
				appointment(regid);
				break;
			case 6:
				patient_archive();
				break;
			default:
				goto EXIT;
		}
	}
EXIT:
	return;
}
