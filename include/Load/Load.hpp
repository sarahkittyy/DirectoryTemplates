#pragma once

#include <algorithm>
#include <cctype>
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
 * @brief The template directory loading application.
 * 
 */
class Load
{
public:
	/**
	 * @brief Constructs the load app instance, loading the settings from the given args.
	 * 
	 * @param argc The amount of cli args.
	 * @param argv The cli args.
	 */
	Load(int argc, char* argv[]);

	/**
	 * @brief Execute the directory loading, given the settings passed during construction.
	 * 
	 * @return int The exit code of the program.
	 */
	int operator()();

private:
	/**
	 * @brief The program options, initialized in the constructor.
	 * 
	 */
	struct
	{
		std::string inputFile;
		std::string outputDir;
		std::unordered_map<std::string, std::string> keys;
	} mOpts;

	/**
	 * @brief Parses the string of comma-delimited key=value pairs,
	 * returning an unordered map.
	 * 
	 * @param in The comma-delimited list of key=value pairs.
	 * @return std::unordered_map<std::string, std::string> A map of key-value pairs. 
	 */
	std::unordered_map<std::string, std::string> parseKeys(std::string in);

	/**
	 * @brief Recursive function to build the directory tree.
	 * 
	 * @param item The json to load.
	 * @param cpath The relative current directory.
	 */
	void
	loadItem(nlohmann::json item, std::filesystem::path cpath);

	/**
	 * @brief Create a file at the given path, and inject the given string into it.
	 * 
	 * @param cpath The path of the item.
	 * @param content The file's content.
	 */
	void injectFile(std::filesystem::path cpath, std::string content);

	/**
	 * @brief Get the value of a given key, for substitution.
	 * 
	 * @param key The substitution key.
	 * @return std::string The replacement.
	 */
	std::string getValue(std::string key);
};