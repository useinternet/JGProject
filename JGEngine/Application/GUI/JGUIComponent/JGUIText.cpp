#pragma once
#include "pch.h"
#include "JGUIText.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "GUI/JGUIFont/JGUIFontManager.h"
using namespace std;



float JGUIText::GetTextWidth(const std::string& font_name, const std::string& txt, float txt_size)
{
	auto  charInfo = JGUI::GetJGUIFontManager()->GetFontCharInfo(font_name, s2ws(txt));
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(font_name);
	float accX = 0.0f;
	float font_size_ratio = txt_size / (float)fontfileInfo.default_font_size;
	for (auto& info : charInfo)
	{
		accX += (info.xadvance * font_size_ratio);
	}
	return accX;
}

void JGUIText::Awake()
{
	auto size = GetTransform()->GetSize();
	ProcessByTextFlag();
}

void JGUIText::Resize(const JGUIResizeEvent& e)
{
	SetText(m_Text);
}
void JGUIText::SetText(const std::string& str)
{
	Reset();
	m_Text = str;
	auto size = GetTransform()->GetSize();


	string   line_str;
	auto  charInfo = JGUI::GetJGUIFontManager()->GetFontCharInfo(m_FontName, s2ws(m_Text));
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);
	float font_size_ratio = m_TextSize / (float)fontfileInfo.default_font_size;
	float accX = 0.0f;



	m_TextMeshs[m_Line] = CreateTextMesh(m_Line);
	for (size_t i = 0; i < m_Text.length(); ++i)
	{
		float t_w = charInfo[i].xadvance * font_size_ratio;
		accX += t_w;

		// 다음줄 이거나 누적 텍스트 가로 길이가 사이즈 가로길이보다 길 경우 다음줄로 넘어간다.
		if (m_Text[i] == '\n' || accX >= size.x)
		{
			m_TextMeshs[m_Line]->SetText(line_str);
			line_str = "";
			accX = 0.0f;
			m_Line++;
			m_TextMeshs[m_Line] = CreateTextMesh(m_Line);
		}
		else
		{
			line_str += m_Text[i];
		}
	}

	m_TextMeshs[m_Line]->SetText(line_str);



	ProcessByHAlign();
	ProcessByVAlign();
	ProcessByTextFlag();
}
void JGUIText::SetFontName(const std::string& name)
{
	m_FontName = name;
}
void JGUIText::SetTextSize(float size)
{
	m_TextSize = size;
	SetText(m_Text);
}

void JGUIText::SetHAlign(EJGUI_Text_HAlignment align)
{
	m_HAlign = align;
	ProcessByHAlign();
}
void JGUIText::SetVAlign(EJGUI_Text_VAlignment align)
{
	m_VAlign = align;
	ProcessByVAlign();
}
void JGUIText::SetTextFlag(EJGUI_Text_Flags flag)
{
	m_TextFlags = flag;
	ProcessByTextFlag();
}

void JGUIText::SetColor(const JColor& color)
{
	for (auto& txt_pair : m_TextMeshs)
	{
		txt_pair.second->SetColor(color);
	}
	m_Color = color;
}



const string& JGUIText::GetFontName() const
{
	return m_FontName;
}
float JGUIText::GetTextSize() const
{
	return m_TextSize;
}
const std::string& JGUIText::GetString() const
{
	return m_Text;
}
EJGUI_Text_HAlignment JGUIText::GetHAlign() const
{
	return m_HAlign;
}
EJGUI_Text_VAlignment JGUIText::GetVAlign() const
{
	return m_VAlign;
}
EJGUI_Text_Flags JGUIText::GetTextFlags() const
{
	return m_TextFlags;
}
float JGUIText::GetTextWidth() const
{
	float width = 0.0f;
	for (auto& mesh : m_TextMeshs)
	{
		width = std::max<float>(width, mesh.second->GetTextWidth());
	}
	return width;
}
float JGUIText::GetTextHeight() const
{
	uint32_t count = m_TextMeshs.size();
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);
	float font_size_ratio = m_TextSize / (float)fontfileInfo.default_font_size;
	float font_height = (float)fontfileInfo.lineHeight * font_size_ratio + m_LineGap;
	float gap = m_LineGap * (count - 1);
	float height = font_height * count;

	return gap + height;
}
void JGUIText::ProcessByTextFlag()
{
	switch (m_TextFlags)
	{
	case JGUI_Text_Flag_None:
		if (m_Border)
		{
			m_Border->SetActive(false);
		}
		break;
	case JGUI_Text_Flag_Border:
		if (m_Border == nullptr)
		{
			m_Border = CreateJGUIComponent<JGUIEmptyRectangle>("TextBorder");
			m_Border->SetThickness(0.5f);
			m_Border->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
			GetTransform()->AttachTransform(m_Border->GetTransform());
		}
		else
		{
			m_Border->SetActive(true);
		}
		break;
	}
}

void JGUIText::ProcessByHAlign()
{
	float x     = 0.0f;
	auto size = GetTransform()->GetSize();
	JVector2 pos;
	switch(m_HAlign)
	{
	case JGUI_Text_HAlignment_Left:
		for (auto& txt_pair : m_TextMeshs)
		{
			pos = txt_pair.second->GetTransform()->GetLocalPosition();
			x = size.x - txt_pair.second->GetTextWidth();
			txt_pair.second->GetTransform()->SetLocalPosition(0.0f, pos.y);
		}
		break;
	case JGUI_Text_HAlignment_Center:

		for (auto& txt_pair : m_TextMeshs)
		{
			pos = txt_pair.second->GetTransform()->GetLocalPosition();
			x = size.x - txt_pair.second->GetTextWidth();
			txt_pair.second->GetTransform()->SetLocalPosition(x * 0.5f, pos.y);
		}
		break;
	case JGUI_Text_HAlignment_Right:
		for (auto& txt_pair : m_TextMeshs)
		{
			pos = txt_pair.second->GetTransform()->GetLocalPosition();
			x = size.x - txt_pair.second->GetTextWidth();
			txt_pair.second->GetTransform()->SetLocalPosition(x, pos.y);
		}
		break;
	}

}
void JGUIText::ProcessByVAlign()
{
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);
	float font_size_ratio = m_TextSize / (float)fontfileInfo.default_font_size;
	float font_height = (float)fontfileInfo.lineHeight * font_size_ratio + m_LineGap;
	float y = m_TextMeshs.size() * font_height;
	JVector2 pos;
	JVector2 size = GetTransform()->GetSize();
	// 100
	// 20
	// 10
	switch (m_VAlign)
	{
	case JGUI_Text_VAlignment_Top:
		for (auto& txt_pair : m_TextMeshs)
		{
			pos = txt_pair.second->GetTransform()->GetLocalPosition();
			y = 0.0f;
			txt_pair.second->GetTransform()->SetLocalPosition(pos.x, y + ((txt_pair.first - 1) * font_height));
		}
		break;
	case JGUI_Text_VAlignment_Center:

		for (auto& txt_pair : m_TextMeshs)
		{
			pos = txt_pair.second->GetTransform()->GetLocalPosition();
			txt_pair.second->GetTransform()->SetLocalPosition(pos.x, (size.y - y) * 0.5f + ((txt_pair.first - 1) * font_height));
		}
		break;
	case JGUI_Text_VAlignment_Bottom:
		for (auto& txt_pair : m_TextMeshs)
		{
			pos = txt_pair.second->GetTransform()->GetLocalPosition();
			txt_pair.second->GetTransform()->SetLocalPosition(pos.x, (size.y - y) + ((txt_pair.first - 1) * font_height));
		}
		break;
	}
}

void JGUIText::Reset()
{
	m_Line = 1;
	for (auto& mesh : m_TextMeshs)
	{
		DestroyJGUIComponent(mesh.second);
	}
	m_TextMeshs.clear();
}

JGUITextMesh* JGUIText::CreateTextMesh(int line)
{
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);
	float font_size_ratio = m_TextSize / (float)fontfileInfo.default_font_size;
	float font_height = (float)fontfileInfo.lineHeight * font_size_ratio;
	auto size = GetTransform()->GetSize();
	auto mesh = CreateJGUIComponent<JGUITextMesh>("TextMesh_" + to_string(m_Line));
	mesh->SetFont(m_FontName);
	mesh->SetFontSize(m_TextSize);
	mesh->GetTransform()->SetLocalPosition(0.0f, (line - 1) * font_height);
	mesh->SetTextRect(size.x, font_height);
	mesh->SetColor(m_Color);
	return mesh;
}
