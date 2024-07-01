#include <iostream>
#include "../head/Menu.h"


using namespace std; 

int main(void)
{
	setlocale(LC_ALL, "Russian");

	Menu menu;
	menu.init();

	system("pause");
	return 0;
}
