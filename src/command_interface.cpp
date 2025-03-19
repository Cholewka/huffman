#include "command_interface.h"
#include "tree_methods.h"

// Assigning some menus additional functionality.
std::unordered_map<Menu, int(*)()> menuFunctions = {
	{ Menu::COMPRESS_FILE, &compress_file_page },
	{ Menu::DECOMPRESS_FILE, &decompress_file_page }
};

Menu get_menu(char index)
{
	switch (index)
	{
	case '0':	return Menu::MAIN_MENU;
	case '1':	return Menu::COMPRESS_FILE;
	case '2':	return Menu::DECOMPRESS_FILE;
	case '3':	return Menu::MANUAL;
	default:	return Menu::NOT_FOUND;
	}
}

void show_menu(Menu menu, std::unordered_map<size_t, std::string>& menus)
{
	system("cls");

	if (menus.count(menu) == 0)
	{
		std::cerr << "Unable to read that option.\n";
		return;
	}

	std::cout << menus[menu];

	// If menu has an assigned function, call it.
	if (menuFunctions.count(menu) != 0)
	{
		std::cout << std::endl;

		// Function may get some data from the input buffer, that's why we need to
		// delete whitespace characters from it.
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

		if (menuFunctions[menu]() == -1)
		{
			std::cerr << "Unable to call an additional function.\n";
			return;
		}
	}
}

int load_menus(const std::string& filepath, std::unordered_map<size_t, std::string>& menus)
{
	menus.clear();

	// Read file from the menu.
	std::ifstream inFile(filepath, std::ios::in);
	if (!inFile.good())
	{
		std::cerr << "Unable to read file \"" << filepath << "\".\n";
		return -1;
	}

	// Read line by line.
	std::string line;
	size_t currentMenu;

	while (std::getline(inFile, line))
	{
		// If line starts with three dashes, that's a line with meu identifier.
		if (line.length() > 3 && line.substr(0, 3) == "---")
		{
			std::string menuIdentifier = line.substr(4, line.length() - 4);
			
			// Check if the identifier is empty.
			if (menuIdentifier.empty() || !(menuIdentifier.find_first_not_of(' ') != std::string::npos))
			{
				std::cerr << "One menu has missing identifier.\n";
				return -1;
			}

			currentMenu = std::stoi(menuIdentifier);
			continue;
		}

		menus[currentMenu].append(line + '\n');
	}

	inFile.close();
	return 0;
}

int compress_file_page()
{
	std::cout << "Enter input file path: ";
	std::string filepath;
	std::getline(std::cin, filepath);
	
	// Open a file.
	std::ifstream inFile(filepath, std::ios::in);
	if (!inFile.good())
	{
		std::cerr << "Unable to read file \"" << filepath << "\".\n";
		return -1;
	}

	// Read into a string.
	std::string text;
	std::string line;
	while (getline(inFile, line))
		text.append(line + '\n');
	inFile.close();

	Node* root = create_huffman_tree(text);
	if (root == nullptr)
	{
		std::cerr << "There is nothing to compress or an error happened.\n";
		return -1;
	}

	std::cout << "Enter output file path: ";
	filepath.clear();
	std::getline(std::cin, filepath);

	if (compress_to_file(root, text, filepath) == -1)
	{
		std::cerr << "Unable to compress file.\n";
		return -1;
	}

	std::cout << "Successfully compressed file. Enter 0 to get back to main menu.\n";
	return 0;
}

int decompress_file_page()
{
	std::cout << "Enter input file path: ";
	std::string filepath;
	std::getline(std::cin, filepath);
	
	std::string decompressedText = decompress_file(filepath);
	if (decompressedText.empty())
	{
		std::cerr << "Unable to decompress file.\n";
		return -1;
	}

	std::cout << "Enter output file path: ";
	filepath.clear();
	std::getline(std::cin, filepath);

	std::ofstream outFile(filepath, std::ios::out);
	if (!outFile.good())
	{
		std::cerr << "Unable to create file \"" << filepath << "\".\n";
		return -1;
	}
	outFile << decompressedText;

	std::cout << "Successfully decompressed file. Enter 0 to get back to main menu.\n";
	outFile.close();
	return 0;
}
