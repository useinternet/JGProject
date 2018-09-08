#pragma once
#include"Widget.h"


class TextComponent;

class ENGINE_EXPORT Text : public Widget
{
private:
	std::unique_ptr<TextComponent> m_Text;
public:
	Text();
	virtual ~Text();
	bool SetFont(const std::string& FontPath, const std::wstring& FontTexturePath);
	void SetText(const std::wstring& text);
};