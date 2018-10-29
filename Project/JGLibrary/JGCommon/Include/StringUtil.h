#pragma once
#include"common.h"
#include<string>

class StringUtil
{
public:
	static bool FindString(const std::string& sentence, const char* str);
	static void DelString(std::string& str, const char* delstr);
	static void DelChar(std::string& str, const char c);
	static void DelBlank(std::string& str);
	static void Path_DelEndPath(std::string& str);
};