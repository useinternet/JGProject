#include "JGClassTypeManager.h"
using namespace std::experimental::filesystem;
using namespace std;
void JGClassTypeManager::SettingEntryPath(const wstring& EntryPath, const wstring& OutputPath)
{
	m_OutputPath = OutputPath;
#ifdef _DEBUG
	m_EntryPath = EntryPath;
#endif
}
bool JGClassTypeManager::SearchAllClassTypeOfDirectory()
{
#ifdef _DEBUG
	bool result = FindHeaderFile(m_EntryPath);
	if (!result)
	{
		return false;
	}
	for (auto& iter : m_vClassHeaderFile)
	{
		result = RegisterClassType(iter.string());
		if (!result)
		{
			return false;
		}
	}
	result = WriteRegisteredClassType();
	if (!result)
	{
		return false;
	}
#endif
	LoadClassType();
	for (auto& iter : m_vConstructClassType)
	{
		std::cout << iter << endl;
	}
	return true;
}

bool JGClassTypeManager::LoadClassType()
{
	fstream fs;
	fs.open(m_OutputPath.string());
	if (!fs.is_open())
	{
		return false;
	}

	char buffer= 'a';


	while (!fs.eof())
	{
		string ClassName;
		while (true)
		{
			fs >> buffer;
			if (buffer == '\\')
			{
				break;
			}
			ClassName += buffer;
		}
		m_vConstructClassType.push_back(ClassName);
	}

	fs.close();
	return true;
}

bool JGClassTypeManager::RegisterClassType(const std::string& FileName)
{
#ifdef _DEBUG
	fstream fs;
	fs.open(FileName);
	string Buffer;
	if (!fs.is_open())
	{
		// 파일 안열림
		return false;
	}

	while (!fs.eof())
	{
		getline(fs, Buffer);
		if (Buffer == "JGClass")
		{
			getline(fs, Buffer);
			for (auto iter = --Buffer.end(); iter >= Buffer.begin(); --iter)
			{
				if (*iter == ' ')
				{
					string ClassName;
					ClassName.append(&(*iter));

					m_vClassTypeToSave.push_back(ClassName);
					break;
				}
			}
		}
	}

	fs.close();
#endif
	return true;
}

bool JGClassTypeManager::WriteRegisteredClassType()
{
#ifdef _DEBUG
	fstream fs;
	fs.open(m_OutputPath);
	if (!fs.is_open())
	{
		return false;
	}

	for (auto& iter : m_vClassTypeToSave)
	{
		fs << (iter+"\\").c_str();
	}
	fs.close();
#endif
	return true;
}

bool JGClassTypeManager::FindHeaderFile(const path& CurrentPath)
{
#ifdef _DEBUG
	if (!is_directory(CurrentPath)) return false;

	for (const auto& entry : directory_iterator{ CurrentPath })

	{
		// 파일일 경우 헤더파일만 저장
		if (is_regular_file(entry.status()))
		{
			auto FileName = entry.path().end();
			wstring FileNameString = (*(--FileName));
			if (IsHeaderFile(FileNameString))
			{
				m_vClassHeaderFile.push_back(entry);
			}
		}
		else if (is_directory(entry.status()))
			FindHeaderFile(entry.path());
	}
#endif
	return true;
}

bool JGClassTypeManager::IsHeaderFile(std::wstring& Path)
{
#ifdef _DEBUG
	wstring EndString;
	EndString.append(&(*(--(--Path.end()))));
	if (EndString == L".h")
	{
		return true;
	}
#endif
	return false;
}
