#pragma once

#include <iostream>
#include <fstream>
#include <fstream>
#include <iostream>
#include <string>





namespace DataIO
{
	template<typename T>
	inline void write(std::ofstream& fout, const T& data)
	{
		size_t size = sizeof(T);
		fout.write(reinterpret_cast<const char*>(&data), size);
	}



	inline void write(std::ofstream& fout, const std::string& str)
	{
		size_t size = str.length();

		fout.write(reinterpret_cast<const char*>(&size), sizeof(size));
		fout.write(reinterpret_cast<const char*>(str.c_str()), size);
	}

	template<typename T>
	inline void read(std::ifstream& fin, T& data)
	{
		size_t size = sizeof(T);

		fin.read(reinterpret_cast<char*>(&data), size);
	}

	inline void read(std::ifstream& fin, std::string& str)
	{
		size_t size = 0;

		fin.read(reinterpret_cast<char*>(&size), sizeof(size_t));
		str.resize(size);
		fin.read(reinterpret_cast<char*>(&str[0]), size);
	}
}


