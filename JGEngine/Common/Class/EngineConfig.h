#pragma once


#include <string>
class EngineConfig
{
private:
	std::string ContentsPath;
	std::string SourcePath;
	std::string EnginePath;
	std::string ConfigPath;

	std::string ImGuiConfig;
	std::string EditorConfig;
public:
	void LoadConfig();
	std::string InContents(const std::string& path) { return ContentsPath + path; }
	std::string InSource(const std::string& path) { return SourcePath + path; }
	std::string InEngine(const std::string& path) { return EnginePath + path; }
	std::string InConfig(const std::string& path) { return ConfigPath + path; }
	std::wstring InContentsW(const std::string& path)
	{ 
		std::wstring wstr;
		std::string str = InContents(path);
		wstr.assign(str.begin(), str.end());
		return wstr;
	}
	std::wstring InSourceW(const std::string& path)
	{ 
		std::wstring wstr;
		std::string str = InSource(path);
		wstr.assign(str.begin(), str.end());
		return wstr;
	}
	std::wstring InEngineW(const std::string& path)
	{ 
		std::wstring wstr;
		std::string str = InEngine(path);
		wstr.assign(str.begin(), str.end());
		return wstr;
	}
	std::wstring InConfigW(const std::string& path)
	{ 
		std::wstring wstr;
		std::string str = InConfig(path);
		wstr.assign(str.begin(), str.end());
		return wstr;
	}

	const std::string GetImGuiConfigFile() const {
		return ImGuiConfig;
	}
	const std::string GetEditorConfigFile() const {
		return EditorConfig;
	}
};