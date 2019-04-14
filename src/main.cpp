#include <algorithm>
#include <iostream>
#include <string>

#include <Load/Load.hpp>
#include <Save/Save.hpp>

/**
 * @brief True if the strings passed are equal regardless of case.
 * 
 */
bool strequ(std::string a, std::string b)
{
	return std::equal(a.begin(), a.end(),
					  b.begin(), b.end(),
					  [](char a, char b) {
						  return tolower(a) == tolower(b);
					  });
}



int main(int argc, char* argv[])
{
	//Assert there is at least the mode option specified.
	if (argc < 2)
	{
		std::cerr << "Not enough arguments!\n"
				  << "Try " << argv[0] << "load/save.\n";
		return -1;
	}
	else if (strequ(argv[1], "save"))
	{
		Save save_app(argc, argv);
		return save_app();
	}
	else if (strequ(argv[1], "load"))
	{
		Load load_app(argc, argv);
		return load_app();
	}
	else
	{
		std::cerr << argv[1] << " not a valid mode! Try save/load.";
		return -1;
	}
}