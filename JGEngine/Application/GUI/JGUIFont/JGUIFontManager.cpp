#include "pch.h"
#include "JGUIFontManager.h"
#include "RenderEngine.h"
using namespace std;

bool JGUIFontManager::FontLoad(const std::string& path)
{
	ifstream fin;
	fin.open(path);

	if (!fin.is_open()) return false;

	std::string bf;
	std::wstring wbf;
	char cf;
	//JGUIFontFileInfo fileInfo;
	JGUIFontInfo fontInfo;

	// 폰트 이름
	Next(fin); fin >> fontInfo.FileInfo.face;
	// 폰트 기본 사이즈
	Next(fin); fin >> fontInfo.FileInfo.default_font_size;
	// 폰트 Bold
	Next(fin); fin >> fontInfo.FileInfo.bold;
	// 폰트 Italic
	Next(fin); fin >> fontInfo.FileInfo.italic;
	// 폰트 charset
	Next(fin); fin >> fontInfo.FileInfo.charset;
	// 폰트 유니코드 여부
	Next(fin); fin >> fontInfo.FileInfo.unicode;
	// 
	Next(fin); fin >> fontInfo.FileInfo.stretchH;
	//
	Next(fin); fin >> fontInfo.FileInfo.smooth;
	//
	Next(fin); fin >> fontInfo.FileInfo.super_sampling;
	// 
	Next(fin); fin >> 
		fontInfo.FileInfo.padding[0] >> cf >>
		fontInfo.FileInfo.padding[1] >> cf >>
		fontInfo.FileInfo.padding[2] >> cf >>
		fontInfo.FileInfo.padding[3];
	//
	Next(fin); fin >> 
		fontInfo.FileInfo.spacing[0] >> cf >>
		fontInfo.FileInfo.spacing[1];
	//
	Next(fin); fin >> fontInfo.FileInfo.outline;
	//
	Next(fin); fin >> fontInfo.FileInfo.lineHeight;
	//
	Next(fin); fin >> fontInfo.FileInfo.base;
	//
	Next(fin); fin >> fontInfo.FileInfo.scaleW;
	//
	Next(fin); fin >> fontInfo.FileInfo.scaleH;
	//
	Next(fin); fin >> fontInfo.FileInfo.pageCount;
	//
	Next(fin); fin >> fontInfo.FileInfo.packed;
	//
	Next(fin); fin >> fontInfo.FileInfo.alphaChnl;
	//
	Next(fin); fin >> fontInfo.FileInfo.redChnl;
	//
	Next(fin); fin >> fontInfo.FileInfo.greenChnl;
	//
	Next(fin); fin >> fontInfo.FileInfo.blueChnl;

	for (int i = 0; i < fontInfo.FileInfo.pageCount; ++i)
	{
		int pageID = 0;
		Next(fin); fin >> pageID; Next(fin); fin >> bf;
		bf = bf.substr(1, bf.size() - 2);
		std::filesystem::path p(path);
		auto filename = p.filename().string();
		bf = path.substr(0, path.size() - filename.size()) + bf;
		fontInfo.FileInfo.page[pageID] = bf;
		RE::RenderEngine::RequestTextureLoad(bf);
	}

	Next(fin); fin >> fontInfo.FileInfo.charCount;


	for (int i = 0; i < fontInfo.FileInfo.charCount; ++i)
	{
		JGUIFontCharInfo charInfo;
		Next(fin); fin >> charInfo.id;
		Next(fin); fin >> charInfo.x;
		Next(fin); fin >> charInfo.y;
		Next(fin); fin >> charInfo.width;
		Next(fin); fin >> charInfo.height;
		Next(fin); fin >> charInfo.xoffset;
		Next(fin); fin >> charInfo.yoffset;
		Next(fin); fin >> charInfo.xadvance;
		Next(fin); fin >> charInfo.pageID;
		Next(fin); fin >> charInfo.chnl;

		fontInfo.CharMap[charInfo.id] = charInfo;
	}
	fin.close();

	m_FontMap[path] = std::move(fontInfo);
	return true;
}

std::vector<JGUIFontCharInfo> JGUIFontManager::GetFontCharInfo(const std::string& path, const std::wstring& str)
{
	std::vector<JGUIFontCharInfo> result;
	for (auto& c : str)
	{
		result.push_back(m_FontMap[path].CharMap[c]);
	}
	return move(result);
}

const JGUIFontInfo& JGUIFontManager::GetFontInfo(const std::string& path)
{
	return m_FontMap[path];
}

const JGUIFontFileInfo& JGUIFontManager::GetFontFileInfo(const std::string& path)
{
	return m_FontMap[path].FileInfo;
	// TODO: 여기에 반환 구문을 삽입합니다.
}

const std::string& JGUIFontManager::GetFontImage(const std::string& path, int id)
{
	return m_FontMap[path].FileInfo.page[id];
}

void JGUIFontManager::Next(std::ifstream& fin)
{
	char c = 0;
	while(c != '=')
		fin >> c;
}
