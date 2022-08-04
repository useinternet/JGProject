#include "FileHelper.h"
#include <fstream>

bool PFileHelper::WriteAllText(const PString& path, const PString& str)
{
	std::ofstream fout;
	fout.open(path.GetRawString());

	if (fout.is_open() == true)
	{
		fout << str.GetRawString();
		fout.close();
		return true;
	}
	else
	{
		fout.close();
		return false;
	}
}

bool PFileHelper::ReadAllText(const PString& path, PString* out_str)
{
	if (out_str == nullptr)
	{
		return false;
	}
	std::ifstream fin;
	fin.open(path.GetRawString());
	if (fin.is_open() == true)
	{
		std::stringstream ss;
		ss << fin.rdbuf();
		
		*out_str = ss.str().c_str();

		fin.close();
		return true;
	}
	else
	{
		fin.close();
		return false;
	}
}
