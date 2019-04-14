#include "Load/Load.hpp"

Load::Load(int argc, char* argv[])
{
	using namespace cxxopts;
	using std::string;

	//Initialize the options parser instance.
	Options options("Template Directory Loader", "Loads a directory from a given template.");

	//Add options

	// clang-format off
	options.add_options()
	("i,in", "The json file to load.", value<std::string>())
	("o,out", "The root directory of the output.", value<std::string>())
	("k,keys", "A string of comma-separated key-value pairs.\nEx:key1=a,key2=b", value<std::string>());
	// clang-format on

	//Parse options.
	auto opts = options.parse(argc, argv);

	//Pass the settings into the internal mOpts instance.
	try
	{
		mOpts.inputFile = opts["in"].as<std::string>();
		mOpts.outputDir = opts["out"].as<std::string>();
		mOpts.keys		= parseKeys(opts["keys"].as<std::string>());
	}
	catch (std::domain_error& e)
	{
		throw std::domain_error("Invalid arguments.\n" + options.help());
	}
}

int Load::operator()()
{
	using nlohmann::json;
	namespace fs = std::filesystem;

	//Load the json file.
	std::ifstream ifile(mOpts.inputFile);
	if (!ifile)
	{
		throw std::runtime_error("Could not load the given json input file.");
	}
	json in;
	ifile >> in;
	ifile.close();

	//Load the json.
	loadItem(in, mOpts.outputDir);

	return 0;
}

void Load::loadItem(nlohmann::json item, std::filesystem::path cpath)
{
	using nlohmann::json;
	namespace fs = std::filesystem;

	//Assert that the directories exist.
	fs::create_directories(cpath);

	std::vector<json> keys = item["keys"];

	//Sort the keys by position.
	std::sort(keys.begin(), keys.end(), [](json& j1, json& j2) {
		return j1.at("position").get<size_t>() < j2.at("position").get<size_t>();
	});

	//Iterate over all files.
	for (auto& file : item["files"])
	{
		//Place the files in the current path.//


		fs::path f(cpath.string() + "/" + file.at("name").get<std::string>());

		//Get the content of the file.
		std::string content = file.at("content");

		//Get the used keys of the file.
		std::vector<std::string> key_uses = file.at("uses");

		//The offset into the string to now insert, as
		//the string will be changing as we append
		//characters to it.
		int charactersAppended = 0;
		//Iterate through all keys.
		for (auto& key : keys)
		{
			//If the string uses it...
			if (std::find(key_uses.begin(), key_uses.end(),
						  key.at("name").get<std::string>()) != key_uses.end())
			{
				//Emplace the key.
				size_t pos		= key.at("position");
				std::string emp = getValue(key.at("name"));
				content.insert(pos + charactersAppended, emp);

				charactersAppended += emp.size();
			}
		}

		injectFile(f, content);
	}

	//For every element in the directory...
	for (auto& elems : item.items())
	{
		//If it's prepended by a '%'..
		if (elems.key()[0] == '%')
		{
			//load it.
			loadItem(elems.value(), cpath.string() + "/" + elems.key().substr(1));
		}
	}
}

void Load::injectFile(std::filesystem::path cpath, std::string content)
{
	//Create the file
	namespace fs = std::filesystem;

	std::ofstream ofile(cpath);

	ofile << content;

	ofile.close();
}

std::string Load::getValue(std::string key)
{
	return mOpts.keys[key];
}

std::unordered_map<std::string, std::string> Load::parseKeys(std::string in)
{
	//Remove all spaces from our input.
	std::string str;
	for (auto& c : in)
	{
		if (!std::isspace(c))
		{
			str.push_back(c);
		}
	}

	//Convert our input to a stringstream.
	std::stringstream ss(str);
	std::vector<std::string> pairs;

	while (ss.good())
	{
		//Grab each comma-separated substring, pushing it
		//into the pairs vector.
		std::string substr;
		getline(ss, substr, ',');
		pairs.push_back(substr);
	}

	std::unordered_map<std::string, std::string> res;

	//For every pair...
	for (auto& pair : pairs)
	{
		//String before the '='...
		std::string key = pair.substr(0, pair.find('='));
		//String after the '='...
		std::string value = pair.substr(pair.find('=') + 1);

		//Append the pair.
		res[key] = value;
	}

	return res;
}