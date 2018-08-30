#pragma once
#include<Windows.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<experimental/filesystem>



/*
Class : JGClassTypeManger 
@param path m_OutputPath : 클래스 타입정보가 담아있는 파일을 저장할 경로
@param vector<string> m_vConstructClassType : 클래스 정보가 담아있는 파일을 불러와 string상태로 저장할 변수 
# - Dubug모드일때만 존재 
@param vector<path> m_vClassHeaderFile : 헤더파일만 경로 저장
@param vector<std::string> m_vClassTypeToSave : 저장할 클래스 타입 배열
@param path m_EntryPath : 검색할 시작 지점 */
class JGClassTypeManager
{
private:
	std::experimental::filesystem::path m_OutputPath;
	std::vector<std::string> m_vConstructClassType;
#ifdef _DEBUG
	//Class type to store
	std::vector<std::experimental::filesystem::path> m_vClassHeaderFile;
	std::vector<std::string> m_vClassTypeToSave;
	std::experimental::filesystem::path m_EntryPath;
#endif
public:
	/*
	Exp : 경로 설정을 한다. 
	@param const wstring EntryPath : 탐색 시작지점 경로 
	@param const wstring OutputPath :  정보 저장할 경로 */
	void SettingEntryPath(const std::wstring& EntryPath, const std::wstring& OutputPath);
	/*
	Exp : 타입을 저장하고 변수 형태로 불러드린다. */
	bool SearchAllClassTypeOfDirectory();
private:
	bool LoadClassType();
	bool RegisterClassType(const std::string& FileName);
	bool WriteRegisteredClassType();
	bool FindHeaderFile(const std::experimental::filesystem::path& CurrentPath);
	bool IsHeaderFile(std::wstring& Path);
};