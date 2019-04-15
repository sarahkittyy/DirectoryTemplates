#include "Save/Save.hpp"

Save::Save(int argc, char* argv[])
{
	using namespace cxxopts;
	using std::string;

	//Initialize the options parser instance.
	Options options("Template Directory Saver", "Saves a directory as a re-useable template");

	//Add options

	// clang-format off
	options.add_options()
	("h,help", "Display this help message.")
	("i,in", "The directory/file to save.", value<std::string>())
	("o,out", "The output name.", value<std::string>())
	("s,start", "The regex-compatible token to indicate the start of a variable.", value<std::string>()->default_value("\\$\\{"))
	("e,end", "The regex-compatible token indicating the end of a variable.", value<std::string>()->default_value("\\}"));
	// clang-format on

	//Parse options.
	auto opts = options.parse(argc, argv);

	if (opts["help"].as<bool>())
	{
		std::cout << options.help();
		std::exit(0);
	}

	//Pass the settings into the internal mOpts instance.
	try
	{
		mOpts.inputName  = opts["in"].as<std::string>();
		mOpts.outputName = opts["out"].as<std::string>();
		mOpts.sTok		 = opts["start"].as<std::string>();
		mOpts.eTok		 = opts["end"].as<std::string>();
	}
	catch (std::domain_error& e)
	{
		throw std::domain_error("Invalid arguments.\n" + options.help());
	}
}

int Save::operator()()
{
	namespace fs = std::filesystem;
	using nlohmann::json;

	//Attempt to load the file/directory
	fs::path item(mOpts.inputName);

	//Create the json output object.
	json out;

	add(out, item);

	//Output the json to a file.
	std::ofstream file(mOpts.outputName);
	if (!file)
	{
		throw std::runtime_error("Could not open output file for writing");
	}
	file << out.dump(1, '\t');
	file.close();

	return 0;
}

void Save::add(nlohmann::json& out, std::filesystem::path item)
{
	namespace fs = std::filesystem;
	using nlohmann::json;

	if (out["keys"].empty())
	{
		out["keys"] = std::vector<std::string>();
	}
	if (out["files"].empty())
	{
		out["files"] = std::vector<json>();
	}

	//If we're saving a directory..
	if (fs::is_directory(item))
	{
		//For every item in that directory...
		for (auto& i : fs::directory_iterator(item))
		{
			//Add it.
			add(out["%" + item.stem().string()], i);
		}
	}
	else   //If it's a singular file.
	{
		// clang-format off
		addFile(out, 
		{
			.path = item.parent_path(),
			.name = item.filename(),
			.content = readFile(item)
		});   // clang-format on
	}
}

void Save::addFile(nlohmann::json& out, Save::File file)
{
	using nlohmann::json;
	namespace fs = std::filesystem;
	using std::regex, std::sregex_iterator, std::smatch, std::regex_replace;

	//Use regex to search for every string surrounded by sTok...eTok.
	regex pattern(mOpts.sTok + "(.*?)" + mOpts.eTok);

	std::set<std::string> used_keys;

	std::string content = file.content;

	//How many characters we've erased in removing keys.
	int charsDeleted = 0;
	for (auto i = sregex_iterator(
			 file.content.begin(),
			 file.content.end(),
			 pattern);
		 i != sregex_iterator();
		 ++i)
	{
		std::string key = (*i)[1];
		// clang-format off
		out["keys"].push_back(
			{
				{"name", key},
				{"position", i->position() - charsDeleted} 
			});
		// clang-format on

		//We got the key, now erase it from the content string.
		content.replace(i->position() - charsDeleted, (*i)[0].str().size(), "");
		//And increment charsDeleted.
		charsDeleted += (*i)[0].str().size();

		used_keys.insert(key);
	}

	//Append the file
	// clang-format off
	out["files"].push_back(
	{
		{"name", file.name},
		{"content", content},
		{"uses", used_keys}
	});
	// clang-format on
}

std::string Save::readFile(std::filesystem::path item)
{
	//Read the file's data.
	std::stringstream data;
	std::ifstream file(item);
	if (!file)
	{
		throw std::runtime_error("Could not open file " + item.string() + " for reading.");
	}
	data << file.rdbuf();
	file.close();
	return data.str();
}