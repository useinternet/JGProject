#pragma once
#include <string>
#include <codecvt>



namespace JG
{
	namespace StringHelper
	{
		inline std::wstring s2ws(const std::string& str)
		{
			using convert_typeX = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_typeX, wchar_t> converterX;

			return converterX.from_bytes(str);
		}
		inline List<std::wstring> s2ws(const List<std::string>& str_list)
		{
			List<std::wstring> result;
			for (auto& str : str_list)
			{
				result.push_back(s2ws(str));
			}
			return result;
		}

		inline std::string ws2s(const std::wstring& wstr)
		{
			using convert_typeX = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_typeX, wchar_t> converterX;

			return converterX.to_bytes(wstr);
		}
		inline List<std::string> ws2s(const List<std::wstring>& str_list)
		{
			List<std::string> result;
			for (auto& str : str_list)
			{
				result.push_back(ws2s(str));
			}
			return result;
		}
		inline String ReplaceAll(const String& message, const String& pattern, const String& replace)
		{
			if (pattern.empty() == true) return message;
			String result = message;
			String::size_type pos = 0;
			String::size_type offset = 0;

			while ((pos = result.find(pattern, offset)) != String::npos)
			{
				result.replace(result.begin() + pos, result.begin() + pos + pattern.size(), replace);
				offset = pos + replace.size();
			}
			return result;
		}

		inline List<String> Split(const String& str, char delimiter)
		{
			List<String> result;
			std::stringstream ss(str);
			String tmp;

			while (std::getline(ss, tmp, delimiter)) {
				result.push_back(tmp);
			}
			return result;
		}

		inline u64 Find(const String& message, const String& pattern, u64 offset, u64 order = 1)
		{
			u64 cnt = 0;
			u64 result = String::npos;
			while (cnt < order )
			{
				u64 pos = message.find(pattern, offset);
				if (pos == String::npos)
				{
					break;
				}
				offset = pos + pattern.length();
				result = pos;
				cnt += 1;
			}
			
			return result;
		}
		inline bool StartWidth(const String& message, const String& pattern)
		{
			if (message.length() < pattern.length())
			{
				return false;
			}
			return std::strncmp(message.c_str(), pattern.c_str(), pattern.length()) == 0;
		}
		inline bool EndWidth(const String& message, const String& pattern)
		{
			if (message.length() < pattern.length())
			{
				return false;
			}

			String endStr = message.substr(message.length() - pattern.length(), pattern.length());
			return std::strncmp(endStr.c_str(), pattern.c_str(), pattern.length()) == 0;
		}

		inline String ToString(std::thread::id id)
		{
			std::stringstream ss;
			ss << id;
			return ss.str();
		}

		inline i32 ToInt(const String& str, i32 default = 0)
		{
			return std::stoi(str, nullptr, default);
		}
	}


	namespace PathHelper
	{
		inline String CombinePath(const String& dest, const String& src)
		{
			String result = "";
			u64 lastIndex = dest.length() - 1;
			if (src[0] != '/' && dest[lastIndex] != '/')
			{
				result = dest + '/' + src;
			}
			else if (src[0] == '/' && dest[lastIndex] == '/')
			{
				result = dest.substr(0, lastIndex) + src;
			}
			else
			{
				result = dest + src;
			}
			return result;
		}
		inline String GetUniqueFileName(const String& filePath)
		{
			auto p = fs::path(filePath);
			String extension = p.extension().string();
			String originFileName = StringHelper::ReplaceAll(p.filename().string(), extension, "");
			String fileName = originFileName;

			String targetDir = p.parent_path().string();



			auto path = CombinePath(targetDir, originFileName + extension);
			i32  cnt = 0;
			bool is_exist = fs::exists(path);
			while (is_exist == true)
			{
				++cnt;
				fileName = originFileName + "_" + std::to_string(cnt);
				path = CombinePath(targetDir, fileName + extension);
				is_exist = fs::exists(path);
			}

			return path;
		}
	}

	


	namespace FileHelper
	{
		inline bool ReadAllText(const String& path, String* out_str)
		{
			if (out_str == nullptr)
			{
				return false;
			}
			std::ifstream fin;
			fin.open(path);
			if (fin.is_open() == true)
			{
				std::stringstream ss;
				ss << fin.rdbuf();
				*out_str = ss.str();

				fin.close();
				return true;
			}
			else
			{
				fin.close();
				return false;
			}
		}

		inline bool WriteAllText(const String& path, const String& txt)
		{
			std::ofstream fout;
			fout.open(path);

			if (fout.is_open() == true)
			{
				fout << txt;
				fout.close();
				return true;
			}
			else
			{
				fout.close();
				return false;
			}
		}


		inline bool ReadAllByte(const String& path, void* data, u64 dataSize)
		{
			std::ifstream fin;
			fin.open(path, std::ios::binary);
			if (fin.is_open() == true)
			{
				fin.read(reinterpret_cast<char*>(data), dataSize);
				fin.close();
				return true;
			}
			else
			{
				fin.close();
				return false;
			}
		}
		inline bool WriteAllByte(const String& path, void* data, u64 dataSize)
		{
			std::ofstream fout;
			fout.open(path, std::ios::binary);

			if (fout.is_open() == true)
			{
				fout.write(reinterpret_cast<const char*>(data), dataSize);
				fout.close();
				return true;
			}
			else
			{
				fout.close();
				return false;
			}
		}


	}

}