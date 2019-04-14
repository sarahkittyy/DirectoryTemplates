#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cxxopts/cxxopts.hpp"
#include "nlohmann/json.hpp"

/**
 * @brief The template directory saving application.
 * 
 */
class Save
{
private:
	/**
	 * @brief The virtual json filesystem file structure.
	 * 
	 */
	struct File
	{
		std::string path;
		std::string name;
		std::string content;
	};

public:
	/**
	 * @brief Constructs the save app instance, loading the settings from the given args.
	 * 
	 * @param argc The amount of cli args.
	 * @param argv The cli args.
	 */
	Save(int argc, char* argv[]);

	/**
	 * @brief Execute the save, given the args in the constructor.
	 * 
	 * @return int The exit code of the program.
	 */
	int operator()();

private:
	/**
	 * @brief The program options, initialized by the constructor.
	 * 
	 */
	struct
	{
		std::string outputName;
		std::string inputName;
		std::string sTok;
		std::string eTok;
	} mOpts;

	/**
	 * @brief Recursive function to add the given item to the json file.
	 * 
	 * @param out The json file to output to. 
	 * @param item The item to add.
	 */
	void add(nlohmann::json& out, std::filesystem::path item);

	/**
	 * @brief Append a file to the given json file, loading the keys.
	 * 
	 * @param out The json output file.
	 * @param file The file to append.
	 */
	void addFile(nlohmann::json& out, File file);

	/**
	 * @brief Return a file's contents as a string.
	 * 
	 * @param item The path of the file.
	 * @return std::string Its contents.
	 */
	std::string readFile(std::filesystem::path item);
};