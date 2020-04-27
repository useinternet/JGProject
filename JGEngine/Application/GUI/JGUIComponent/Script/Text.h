#pragma once
#include "GUI/JGUIComponent/Element/JGUIRenderer.h"

namespace RE
{
	class ReGuiMesh;
}

class  JGUIFontManager;
struct JGUIFontFileInfo;

class JGENGINE_API Text : public JGUIRenderer
{
	class  TextMesh;
public:
	static float GetTextWidth(const std::string& fontname, float fontsize, const std::string& text);
protected:
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Destroy() override;
	virtual void Tick(float tick) override;
	virtual void ChangeParent(JGUIObject* prevparent) override;
	virtual void Init();
public:
	void SetText(std::string text);
	std::string ToString() const;
public:
	const std::string& GetFontName() const {
		return m_Font;
	}
	float GetFontSize() const {
		return m_FontSize;
	}

	const JColor& GetColor() const {
		return m_Color;
	}
	EJGUI_Text_HAlignment GetHAlign() const {
		return m_HAlign;
	}
	EJGUI_Text_VAlignment GetVAlign() const {
		return m_VAlign;
	}
	uint32_t GetLine() {
		return (uint32_t)m_TextMeshByLine.size();
	}

	void SetColor(const JColor& color);
	void SetFontSize(float font_size);
	void SetHAlign(EJGUI_Text_HAlignment align);
	void SetVAlign(EJGUI_Text_VAlignment align);




	float GetTextWidth() const;
private:
	void UpdateIOffset();
	void MergeTextMesh(std::vector<JGUIVertex>& v, std::vector<uint32_t>& i);
private:
	std::string      m_Font   = JGUI::GetDefaultFontName();
	JColor           m_Color  = JColor::White();
	JGUIFontManager* m_FM     = nullptr;

	std::map<uint32_t, std::shared_ptr<TextMesh>> m_TextMeshByLine;
	std::shared_ptr<RE::ReGuiMesh> m_ReMesh;
	float m_FontSize = 20.0f;
	EJGUI_Text_HAlignment m_HAlign = JGUI_Text_HAlignment_Left;
	EJGUI_Text_VAlignment m_VAlign = JGUI_Text_VAlignment_Top;
private:
	class TextMesh
	{
	public:
		void Update();
		bool IsDirty() const {
			return is_dirty;
		}
		void SendDirty() {
			prev_text = "";
		}
		void Flush() {
			is_dirty = false;
		}
	public:
		JGUIFontManager* fm = nullptr;
		Text* owner = nullptr;
		std::string      text;
		uint32_t         ioffset = 0;
		uint32_t         line = 0;

		std::vector<JGUIVertex> v_array;
		std::vector<uint32_t>   i_array;
	private:

		std::string prev_text = "";
		bool is_first = true;
		bool is_dirty = false;
	};
};

