#pragma once

#include "GUI/JGUIObject/JGUIComponent.h"


namespace RE
{
	class RenderItem;
}
class JGUIRenderItem;
class JGUIShape : public JGUIComponent
{

protected:
	virtual void Awake() override;
	virtual void Destroy() override;
	virtual void Tick(const JGUITickEvent& e) override;
public:
	virtual void SetActive(bool is_active) override;
	void SetClipFlag(EJGUI_Clip_Flags flags);
	EJGUI_Clip_Flags GetClipFlag() const {
		return m_ClipFlag;
	}
protected:
	virtual void InitRenderItem(RE::RenderItem* ri) {}
	void CreateRI(JGUIWindow* drawing_win);
	JGUIRenderItem* GetRenderItem() const {
		return m_RI;
	}
private:
	JGUIRenderItem* m_RI = nullptr;

public:

	EJGUI_Clip_Flags m_ClipFlag = JGUI_Clip_Flag_None;
};


class JGUIWindowTexture : public JGUIShape
{
public:
	void Bind(const std::string& moduleKey);
	void UnBind();
protected:
	virtual void InitRenderItem(RE::RenderItem* ri) override;
	virtual void Tick(const JGUITickEvent& e) override;
	virtual void Resize(const JGUIResizeEvent& e) override;
protected:
	std::string m_ModuleKey;
};


class JGUIImage : public JGUIShape
{

protected:
	virtual void InitRenderItem(RE::RenderItem* ri) override;
	virtual void Resize(const JGUIResizeEvent& e)   override;
public:
	void SetImage(const std::string& path);
	void SetColor(const JColor& color);
	void SetColor(float r, float g, float b, float a);

	const JColor& GetColor() const {
		return m_Color;
	}
private:
	JColor m_Color = JColor(1, 1, 1, 1);
	std::string m_ImageSource = "none";
};



class JGUIRectangle : public JGUIShape
{
protected:
	virtual void InitRenderItem(RE::RenderItem* ri) override;
	virtual void Resize(const JGUIResizeEvent& e) override;
public:
	void SetColor(const JColor& color);
	void SetColor(float r, float g, float b, float a);

	const JColor& GetColor() const {
		return m_Color;
	}
private:
	JColor m_Color = JColor(1, 1, 1, 1);
};



class JGUIEmptyRectangle : public JGUIShape
{
protected:
	virtual void InitRenderItem(RE::RenderItem* ri) override;
	virtual void Resize(const JGUIResizeEvent& e) override;
public:
	void SetColor(const JColor& color);
	void SetColor(float r, float g, float b, float a);
	void SetThickness(float thick);

	const JColor& GetColor() const {
		return m_Color;
	}
	float GetThickness() const {
		return m_Thick;
	}
private:
	JColor m_Color = JColor(1, 1, 1, 1);
	float  m_Thick = 5.0f;
};


class JGUITextMesh : public JGUIShape
{
protected:
	virtual void Awake() override;
	virtual void InitRenderItem(RE::RenderItem* ri) override;
public:
	void SetFont(const std::string& str);
	const std::string& GetFontName() const {
		return m_FontName;
	}
	void SetText(const std::string& str);
	void AddText(const std::string& str);
	void Insert(int idx, const std::string& str);
	void Modify(int idx, const std::string& str);
	void RemoveFirstOf(const std::string& str);
	void RemoveLastOf(const std::string& str);
	void RemoveAll(const std::string& str);
	void RemoveRange(int start_pos, int end_pos);
	void RemoveBack();

	// 
	uint32_t Length() const;
	//
	void  SetFontSize(float size);
	float GetFontSize()
	{
		return m_FontSize;
	}

	const std::string& GetString() const {
		return m_Text;
	}
	void SetTextRect(float width, float height);
	const JGUIRect& GetLocalTextRect() const {
		return m_TextRect;
	}
	//
	void SetColor(const JColor& color);
	void SetColor(float r, float g, float b, float a);
	const JColor& GetColor() const {
		return m_Color;
	}
	JVector2 GetTextLastPos(int n);
	JVector2 GetTextPos(int n);
	float    GetTextWidth() const;
	void PushRight(int last_index_located);
	void PushRight(float off);
	void PushLeft(int start_index_located);
	void PushLeft(float off);
	void PushUp(int start_line_located);
	void PushDown(int end_line_located);
	//

	void SetTextFlags(EJGUI_Text_Flags flags);
	void SetTextDrawing(EJGUI_Text_Drawing type);

	 
	EJGUI_Text_Flags GetTextFlags() const       { return m_TextFlags; }
	EJGUI_Text_Drawing GetTextDrawing() const   { return m_DrawingType; }



public:
	JGUITextMesh& operator=(const std::string& str);
	JGUITextMesh& operator+=(const std::string& str);
protected:
	std::pair<uint32_t, JVector2> ConvertVertex(
		uint32_t ioffset, JVector2 start_pos, const std::string& str,
		std::vector<JGUIVertex>& v, std::vector<uint32_t>& i, std::vector<JVector2>& pos);
	void UpdateTextSize(const std::string& str);
	void UpdateTextFlags();
private:
	float GetTextWidth(const std::string& str) const;
private:
	std::string m_FontName = JGUI::GetDefaultFontName();
	std::string m_Text = "";
	float       m_FontSize = 16;
	float       m_TextWidth = 0.0f;
	int         m_LineCount = 0;
	float       m_TextWidthDelta = 0.0f;
	std::vector<JVector2> m_TextPosMap;

	JGUIRect m_TextRect;
	uint32_t m_IOffset = 0;
	JVector2 m_StartPos;
	int  m_VertexCount = 4;
	int  m_IndexCount = 6;

	JColor m_Color = JColor(1, 1, 1, 1);



	JGUIEmptyRectangle* m_Border = nullptr;
	JGUIRectangle*      m_Rect = nullptr;
	// ÇÃ·¡±×
	EJGUI_Text_Flags      m_TextFlags   = JGUI_Text_Flag_None;
	EJGUI_Text_Drawing    m_DrawingType = JGUI_Text_Drawing_NextLine;

	float m_HAlignmentDelta = 0.0f;
	float m_VAlignmentDelta = 0.0f;
};



