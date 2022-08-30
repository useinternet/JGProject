#include "FileHelper.h"
#include "Misc/Log.h"
#include <fstream>

static const char EXTENSION_TOKEN = '.';
static const char PATH_SEP_TOKEN1 = '/';
static const char PATH_SEP_TOKEN2 = '\\';

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

bool PFileHelper::CopyFileOrDirectory(const PString& dest, const PString& src)
{
	std::error_code errCode;

	if (IsDirectory(dest) == true)
	{
		fs::copy(dest.GetRawString(), src.GetRawString(), fs::copy_options::recursive, errCode);
	}
	else
	{
		fs::copy_file(dest.GetRawString(), src.GetRawString(), fs::copy_options::overwrite_existing, errCode);
	}

	if (errCode.value() != 0)
	{
		JG_LOG(Core, ELogLevel::Error, "Fail Copy[%s -> %s]: %s", dest, src, errCode.message().c_str());
		return false;
	}

	return true;
}

bool PFileHelper::RemoveFileOrDirectory(const PString& path)
{
	if (IsDirectory(path) == true)
	{
		return fs::remove_all(path.GetRawString());
	}
	else
	{
		return fs::remove(path.GetRawString());
	}
}

bool PFileHelper::Exists(const PString& inPath)
{
	return fs::exists(inPath.GetRawString());
}

void PFileHelper::FileName(const PString& inPath, PString* outStr)
{
	if (outStr == nullptr)
	{
		return;
	}

	uint64 pos = inPath.FindLastOf(PATH_SEP_TOKEN1);
	if (pos == PString::NPOS)
	{
		pos = inPath.FindLastOf(PATH_SEP_TOKEN2);
	}

	inPath.SubString(*outStr, pos + 1);
}

void PFileHelper::FileNameOnly(const PString& inPath, PString* outStr)
{
	if (outStr == nullptr)
	{
		return;
	}

	PString path = inPath;

	uint64 pos = path.FindLastOf(EXTENSION_TOKEN);
	if (pos != PString::NPOS)
	{
		path.SubString(path, 0, pos);
	}

	FileName(path, outStr);
}

void PFileHelper::FilePathOnly(const PString& inPath, PString* outStr)
{
	if (outStr == nullptr)
	{
		return;
	}

	uint64 pos = inPath.FindLastOf(PATH_SEP_TOKEN1);
	if (pos == PString::NPOS)
	{
		pos = inPath.FindLastOf(PATH_SEP_TOKEN2);
	}

	inPath.SubString(*outStr, 0, pos);
}

bool PFileHelper::FileExtension(const PString& inPath, PString* outStr)
{
	if (outStr == nullptr)
	{
		return false;
	}

	uint64 pos = inPath.FindLastOf(EXTENSION_TOKEN);
	if (pos == PString::NPOS)
	{
		return false;
	}

	inPath.SubString(*outStr, pos);

	return true;
}

void PFileHelper::FileListInDirectory(const PString& inDir, PList<PString>* outFileList)
{
	if (outFileList == nullptr)
	{
		return;
	}

	outFileList->clear();

	fs::directory_iterator dirIter = fs::directory_iterator(inDir.GetRawString());
	for (auto& fileIter : dirIter)
	{
		PString filePath = fs::path(fileIter).string().c_str();
		NormalizePath(&filePath);
		outFileList->push_back(filePath);
	}
}

bool PFileHelper::IsDirectory(const PString& directoryName)
{
	return fs::is_directory(directoryName.GetRawString());
}

void PFileHelper::CombinePath(const PString& p1, const PString& p2, PString* outStr)
{
	if (outStr == nullptr)
	{
		return;
	}

	*outStr = PString();
	outStr->Append(p1);

	uint64 p1LastPos = p1.Length() - 1;
	
	if (p1[p1LastPos] != PATH_SEP_TOKEN1 && p1[p1LastPos] != PATH_SEP_TOKEN2)
	{
		outStr->Append("/");
	}

	outStr->Append(p2);
}

void PFileHelper::NormalizePath(PString* outPath)
{
	if (outPath == nullptr)
	{
		return;
	}

	outPath->ReplaceAll(PATH_SEP_TOKEN2, PATH_SEP_TOKEN1);
}

void PFileHelper::AbsolutePath(const PString& inPath, PString* outPath)
{
	if (outPath == nullptr)
	{
		return;
	}

	(*outPath) = fs::absolute(inPath.GetRawString()).string().c_str();
	NormalizePath(outPath);
}

const PString& PFileHelper::EngineDirectory()
{
	static PString enginePath = "../../";

	return enginePath;
}

const PString& PFileHelper::EngineBuildDirectory()
{
	static PString engineBuildPath;
	if (engineBuildPath.Empty())
	{
		CombinePath(EngineDirectory(), "Build", &engineBuildPath);
	}

	return engineBuildPath;
}

const PString& PFileHelper::EngineContentsDirectory()
{
	static PString engineContentsPath;
	if (engineContentsPath.Empty())
	{
		CombinePath(EngineDirectory(), "Contents", &engineContentsPath);
	}

	return engineContentsPath;
}

const PString& PFileHelper::EngineConfigDirectory()
{
	static PString engineConfigPath;
	if (engineConfigPath.Empty())
	{
		CombinePath(EngineDirectory(), "Config", &engineConfigPath);
	}

	return engineConfigPath;
}

const PString& PFileHelper::EngineSourceDirectory()
{
	static PString engineSourcePath;
	if (engineSourcePath.Empty())
	{
		CombinePath(EngineDirectory(), "Source", &engineSourcePath);
	}

	return engineSourcePath;
}

const PString& PFileHelper::EngineEditorSourceDirectory()
{
	static PString engineEditorSourcePath;
	if (engineEditorSourcePath.Empty())
	{
		CombinePath(EngineSourceDirectory(), "Editor", &engineEditorSourcePath);
	}

	return engineEditorSourcePath;
}

const PString& PFileHelper::EngineProgramsSourceDirectory()
{
	static PString engineProgramsSourcePath;
	if (engineProgramsSourcePath.Empty())
	{
		CombinePath(EngineSourceDirectory(), "Programs", &engineProgramsSourcePath);
	}

	return engineProgramsSourcePath;
}

const PString& PFileHelper::EngineRuntimeSourceDirectory()
{
	static PString engineRuntimeSourcePath;
	if (engineRuntimeSourcePath.Empty())
	{
		CombinePath(EngineSourceDirectory(), "Runtime", &engineRuntimeSourcePath);
	}

	return engineRuntimeSourcePath;
}

const PString& PFileHelper::EngineThirdPartyDirectory()
{
	static PString engineThirdPartyPath;
	if (engineThirdPartyPath.Empty())
	{
		CombinePath(EngineSourceDirectory(), "ThirdParty", &engineThirdPartyPath);
	}

	return engineThirdPartyPath;
}
