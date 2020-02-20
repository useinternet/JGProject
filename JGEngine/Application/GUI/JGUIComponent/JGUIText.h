#pragma once
#include "GUI/JGUIObject/JGUIComponent.h"


// mesh 여러개 

// 한줄에 mesh 하나 map<line, mesh> 로 저장


class JGUITextMesh;
class JGUIText : public JGUIComponent
{
protected:
	virtual void Awake() override;
	virtual void Resize(const JGUIResizeEvent& e) override;
public:
	void SetText(const std::string& str);
	void SetFontName(const std::string& name);
	void SetTextSize(float size);
	void SetHAlign(EJGUI_Text_HAlignment align);
	void SetVAlign(EJGUI_Text_VAlignment vlign);
	void SetTextFlag(EJGUI_Text_Flags flag);
	void SetColor(const JColor& color);
	const std::string& GetFontName() const;
	float GetTextSize() const;
	const std::string& GetString()    const;
	EJGUI_Text_HAlignment GetHAlign() const;
	EJGUI_Text_VAlignment GetVAlign() const;
	EJGUI_Text_Flags GetTextFlags()   const;
private:
	void ProcessByTextFlag();
	void ProcessByHAlign();
	void ProcessByVAlign();
	void Reset();
	JGUITextMesh* CreateTextMesh(int line);
private:
	std::map<int, JGUITextMesh*>   m_TextMeshs;
	class JGUIEmptyRectangle*      m_Border = nullptr;

	EJGUI_Text_Flags      m_TextFlags = JGUI_Text_Flag_None;
	EJGUI_Text_HAlignment m_HAlign    = JGUI_Text_HAlignment_Left;
	EJGUI_Text_VAlignment m_VAlign    = JGUI_Text_VAlignment_Top;
	//
	int   m_Line = 1;
	float m_LineGap = 0.0f;
	std::string m_FontName = JGUI::GetDefaultFontName();
	float       m_TextSize = 16.0f;
	//
	std::string m_Text;
	JColor      m_Color = { 1.0f,1.0f,1.0f,1.0f };
};