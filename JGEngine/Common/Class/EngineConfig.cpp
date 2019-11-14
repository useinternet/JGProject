#include "pch.h"
#include "EngineConfig.h"
//
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/reader.h"

using namespace std;
using namespace rapidjson;
void EngineConfig::LoadConfig()
{
	ifstream fin;
	fin.open("..\\JGEngine\\JGEngineSetting.json");
	assert(fin.is_open() && "failed load engine config file");

	fin.seekg(0, std::ios::end);
	uint32_t file_size = fin.tellg();
	fin.seekg(0, std::ios::beg);

	std::vector<char> json(file_size);

	fin.read(&json[0], file_size);
	fin.close();




	rapidjson::Document config;
	config.Parse(json.data());
	assert(config.IsObject() && "failed load engine config file");


	// Path
	{
		ContentsPath = config["ContentsPath"].GetString();
		SourcePath = config["SourcePath"].GetString();
		EnginePath = config["EnginePath"].GetString();
		ConfigPath = config["ConfigPath"].GetString();

	}

	

	// Config File
	{
		ImGuiConfig = config["ImGuiConfig"].GetString();
		EditorConfig = config["EditorConfig"].GetString();
	}


}
