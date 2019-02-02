#define MAIN_H
#include <gui.h>

int main(void)
{
	setlocale(LC_ALL, "");
	char *credentials[2];
	initscr();
	cbreak();
	noecho();
	login(credentials);
	patient_interface();
	nocbreak();
	echo();
	endwin();
	return 0;

}
