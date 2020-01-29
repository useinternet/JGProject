#pragma once


#include "GUI/JGUI.h"

namespace RE
{
	
}
struct JGUIFontCharInfo
{
	int id = 0;
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int xoffset = 0;
	int yoffset = 0;
	int xadvance = 0;
	int pageID = 0;
	int chnl = 0;
};
struct JGUIFontFileInfo
{
	std::string face;
	int default_font_size = 0;
	bool bold = false;
	bool italic = false;
	std::string charset;
	bool unicode = false;
	int stretchH = 0;
	bool smooth = false;
	int super_sampling = 0;
	int padding[4];
	int spacing[2];
	int outline = 0;
	int lineHeight = 32;
	int base = 27;
	float scaleW = 0.0f;
	float scaleH = 0.f;
	int packed = 0;
	int alphaChnl = 0;
	int redChnl = 0;
	int greenChnl = 0;
	int blueChnl = 0;
	int pageCount = 0;
	std::map<int, std::string> page;
	int charCount = 0;

};
struct JGUIFontInfo
{
	JGUIFontFileInfo FileInfo;
	std::unordered_map<int, JGUIFontCharInfo> CharMap;
};
class JGUIFontManager
{
public:
	bool FontLoad(const std::string& path);
	std::vector<JGUIFontCharInfo> GetFontCharInfo(const std::string& path, const std::wstring& str);
	const JGUIFontInfo&           GetFontInfo(const std::string& path);
	const JGUIFontFileInfo&       GetFontFileInfo(const std::string& path);
	const std::string&            GetFontImage(const std::string& path, int id = 0);
private:
	void Next(std::ifstream& fin);
private:
	std::unordered_map<std::string, JGUIFontInfo> m_FontMap;
};