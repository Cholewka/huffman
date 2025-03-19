// File contains functions creating console interface.

#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>

// An enumeration assigning menu name to variables.
enum Menu
{
	MAIN_MENU = 0,
	COMPRESS_FILE = 1,
	DECOMPRESS_FILE = 2,
	MANUAL = 3,
	NOT_FOUND = -1
};

Menu get_menu(char index);
void show_menu(Menu menu, std::unordered_map<size_t, std::string>& menus);

// The function loads menus from a file and assigns them according to their
// identifiers. Returns -1 in case of an error.
int load_menus(const std::string& filepath, std::unordered_map<size_t, std::string>& menus);

// Functions containing implementation of some submenus.
int compress_file_page();
int decompress_file_page();