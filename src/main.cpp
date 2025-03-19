#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#include "tree_methods.h"
#include "command_interface.h"

int main()
{
	std::unordered_map<size_t, std::string> menus;
	if (load_menus("menus.txt", menus) == -1)
	{
		std::cerr << "Unable to load menus.\n";
		return 1;
	}

	show_menu(Menu::MAIN_MENU, menus);

	char option = 0;
	while (true)
	{
		std::cout << "\nEnter an option (or Q to exit): ";
		std::cin >> option;

		if (option == 'Q')
			return 0;
		
		show_menu(get_menu(option), menus);
	}

	return 0;
}