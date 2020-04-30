#include "pch.h"
#include "Text.h"

#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIObject/JGUIElement.h"


#include "GUI/JGUIComponent/JGUICamera.h"
#include "GUI/JGUIComponent/JGUITransform.h"
#include "GUI/JGUIComponent/Element/JGUICanvas.h"


#include "GUI/JGUIFont/JGUIFontManager.h"




#include "RenderEngine.h"
#include "Object/ReObject/ReMaterial.h"
#include "Object/ReObject/RenderItem.h"
#include "Object/ReObject/ReMesh.h"
#include "Object/DxObject/Resource.h"
#include "Object/Shader/ShaderModule.h"


using namespace std;
JGUI_REGISTER_COMPONENT(Text)
float Text::GetTextWidth(const std::string& fontname, float fontsize, const std::string& text)
{
	auto fm = JGUI::GetJGUIFontManager();
	auto fontInfo     = fm->GetFontInfo(fontname);
	auto fontFileInfo = fm->GetFontFileInfo(fontname);
	auto charInfo     = fm->GetFontCharInfo(fontname, s2ws(text));


	float font_size_ratio = fontsize / (float)fontFileInfo.default_font_size;
	float width = 0.0f;
	for (auto& c_pair : charInfo)
	{
		width += c_pair.xadvance * font_size_ratio;
	}
	return width;
}
void Text::Awake()
{
	JGUIRenderer::Awake();
	m_FM = JGUI::GetJGUIFontManager();
	GetOwner()->BindResizeFunc([&](const JGUIResizeEvent& e)
	{
		std::string text = replaceAll(ToString(), "\n", "");
		SetText(text);
	});
}

void Text::Start()
{

}

void Text::Destroy()
{
	JGUIRenderer::Destroy();
}

void Text::Tick(float tick)
{
	if (m_RenderItem)
	{
		if (m_RenderItem->GetMaterial() == nullptr) Init();
	}
	bool is_dirty = false;
	
	for (auto& tm_pair : m_TextMeshByLine)
	{
		tm_pair.second->Update();
		if (tm_pair.second->IsDirty()) is_dirty = true;
	}

	if (is_dirty)
	{
		std::vector<JGUIVertex> v; std::vector<uint32_t> i;
		MergeTextMesh(v, i);
		if (!v.empty() && !i.empty()) m_ReMesh->Create(v, i);
	}
	JGUIRenderer::Tick(tick);
}

void Text::ChangeParent(JGUIObject* prevparent)
{
	JGUIRenderer::ChangeParent(prevparent);
	Init();
}

void Text::Init()
{
	JVector2 size;
	if (GetOwner())
	{
		if (m_RenderItem) m_RenderItem->SetPriority(GetOwner()->GetRISortingOrder());
		size = GetOwner()->GetTransform()->GetSize();
	}
	if (m_RenderItem)
	{
		m_RenderItem->SetMaterial(RE_GUI_OneTextureDefault);
		m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);

		// 폰트 텍스쳐
		m_RenderItem->GetMaterial()->SetTexture("Image", m_FM->GetFontImage(m_Font));


		// 이부분
		if (m_ReMesh == nullptr)
		{
			m_ReMesh = make_shared<RE::ReGuiMesh>();
			std::vector<JGUIVertex> v; std::vector<uint32_t> i;
			MergeTextMesh(v, i);
			if(!v.empty() && !i.empty()) m_ReMesh->Create(v, i);
			
		}
		m_RenderItem->SetMesh(m_ReMesh);
	}
}

void Text::SetText(std::string text)
{
	size_t   c_p     = 0;
	uint32_t line    = 0;
	uint32_t ioffset = 0;
	auto size = GetTransform()->GetSize();
	auto fontFileInfo = m_FM->GetFontFileInfo(m_Font);
	float font_size_ratio = m_FontSize / (float)fontFileInfo.default_font_size;


	m_TextMeshByLine.clear();
	if (m_ReMesh)
	{
		m_ReMesh->Reset();
	}

	//m_ReMesh = make_shared<RE::ReGuiMesh>();
	while (c_p != string::npos)
	{
		c_p = text.find('\n', 0);
		float text_width = 0.0f;
		bool  is_text_over = false;
		auto text_mesh = make_unique<TextMesh>();
		text_mesh->owner = this;
		text_mesh->fm = m_FM;
		text_mesh->line = line;
		if (c_p == 0)
		{
			text = text.substr(c_p + 1);
		}
		else if (c_p == string::npos)
		{
			string sub_str;
			for (size_t i = 0; i<text.length(); ++i)
			{
				wstring w;
				auto ipos = text.begin() + i;
				w.insert(w.end(), ipos, ipos + 1);
				auto info = m_FM->GetFontCharInfo(m_Font, w);
				text_width += info[0].xadvance * font_size_ratio;
				if (text_width >= size.x)
				{
					c_p = i;
					sub_str = text.substr(c_p);
					is_text_over = true;
					break;
				}
			}
			// 여기서 텍스트가 넘치는지 확인 cp조절
			if (c_p != string::npos)
			{

				text_mesh->text = text.substr(0, c_p);
				text = text.substr(c_p);
			}
			else
			{
				text_mesh->text = text;
				text = "";
			}
		}
		else
		{
			//string sub_str = text.substr(0, c_p);
			string sub_str;
			size_t over_c_p = c_p;
			for (size_t i = 0; i < c_p; ++i)
			{
				wstring w;
				auto ipos = text.begin() + i;
				w.insert(w.end(), ipos, ipos + 1);
				auto info = m_FM->GetFontCharInfo(m_Font, w);
				text_width += info[0].xadvance * font_size_ratio;
				if (text_width >= size.x)
				{
					is_text_over = true;
					over_c_p = i;
					sub_str = text.substr(0, over_c_p);
					break;
				}
			}
			if (over_c_p == c_p)
			{
				sub_str = text.substr(0, c_p);
				text_mesh->text = sub_str;
				text = text.substr(c_p + 1);
			}
			else
			{
				text_mesh->text = sub_str;
				c_p = over_c_p;
				text = text.substr(c_p);
			}
		}
		m_TextMeshByLine[line++] = move(text_mesh);
	}
	UpdateIOffset();
}

std::string Text::ToString() const
{
	string result;
	size_t idx = 0;
	for (auto& text_pair : m_TextMeshByLine)
	{
		result += text_pair.second->text;
		if (idx + 1 != m_TextMeshByLine.size()) result += "\n";
		++idx;
	}
	return result;
}

void Text::SetColor(const JColor& color)
{
	if (m_Color == color) return;
	m_Color = color;
	if (m_RenderItem && m_RenderItem->GetMaterial())
	{
		m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	}
	for (auto& text_pair : m_TextMeshByLine)
	{
		text_pair.second->SendDirty();
	}
}

void Text::SetFontSize(float font_size)
{
	if (m_FontSize == font_size) return;
	m_FontSize = font_size;
	for (auto& text_pair : m_TextMeshByLine)
	{
		text_pair.second->SendDirty();
	}
}

void Text::SetHAlign(EJGUI_Text_HAlignment align)
{
	if (m_HAlign == align) return;
	m_HAlign = align;
	for (auto& text_pair : m_TextMeshByLine)
	{
		text_pair.second->SendDirty();
	}
}

void Text::SetVAlign(EJGUI_Text_VAlignment align)
{
	if (m_VAlign == align) return;
	m_VAlign = align;
	for (auto& text_pair : m_TextMeshByLine)
	{
		text_pair.second->SendDirty();
	}
}

float Text::GetTextWidth() const
{
	auto txt_size = GetTransform()->GetSize();
	auto fontInfo = m_FM->GetFontInfo(GetFontName());
	auto fontFileInfo = m_FM->GetFontFileInfo(GetFontName());
	string text = ToString();
	auto charInfo = m_FM->GetFontCharInfo(GetFontName(), s2ws(text));


	float font_size_ratio = GetFontSize() / (float)fontFileInfo.default_font_size;
	float width = 0.0f;
	for (auto& c_pair : charInfo)
	{
		width += c_pair.xadvance * font_size_ratio;
		if (width > txt_size.x)
		{
			width -= c_pair.xadvance * font_size_ratio;
			break;
		}
	}
	return width;
}

void Text::UpdateIOffset()
{
	uint32_t ioffset = 0;
	for (auto& tm_pair : m_TextMeshByLine)
	{
		tm_pair.second->ioffset = ioffset;
		ioffset += tm_pair.second->text.length() * 4;
	}
}

void Text::MergeTextMesh(std::vector<JGUIVertex>& v, std::vector<uint32_t>& i)
{
	for (auto& tm_pair : m_TextMeshByLine)
	{
		if (!tm_pair.second->IsDirty()) return;

		v.insert(v.end(), tm_pair.second->v_array.begin(), tm_pair.second->v_array.end());
		i.insert(i.end(), tm_pair.second->i_array.begin(), tm_pair.second->i_array.end());
		tm_pair.second->Flush();
	}
}



void Text::TextMesh::Update()
{
	if (text == prev_text && !is_first) return;
	is_first = false;
	v_array.clear();
	i_array.clear();

	auto fontInfo     = fm->GetFontInfo(owner->GetFontName());
	auto fontFileInfo = fm->GetFontFileInfo(owner->GetFontName());
	auto charInfo     = fm->GetFontCharInfo(owner->GetFontName(), s2ws(text));

	float font_size_ratio = owner->GetFontSize() / (float)fontFileInfo.default_font_size;
	float font_height     = (float)fontFileInfo.lineHeight * font_size_ratio;

	JVector2 start_pos;
	start_pos.y -= line * font_height;

	uint32_t ioff = ioffset;


	float text_width = 0.0f;
	for (auto& info : charInfo)
	{
		text_width += (info.xadvance * font_size_ratio);
	}

	JVector2 owner_size = owner->GetTransform()->GetSize();
	float align_offset_x = 0.0f;
	float align_offset_y = 0.0f;
	uint32_t owner_line = owner->GetLine();
	switch (owner->GetHAlign())
	{
	case JGUI_Text_HAlignment_Left:
		align_offset_x = owner_size.x * -0.5f;
		break;
	case JGUI_Text_HAlignment_Center:
		align_offset_x = text_width * -0.5f;
		break;
	case JGUI_Text_HAlignment_Right:
		align_offset_x = (owner_size.x * 0.5f) - text_width;
		break;
	}

	float first_line_y = (owner_line) * font_height * 0.5f;
	switch (owner->GetVAlign())
	{
	case JGUI_Text_VAlignment_Top:
		align_offset_y = owner_size.y * 0.5f;
		break;
	case JGUI_Text_VAlignment_Center:
		align_offset_y = -first_line_y + (owner_line * font_height);
		break;
	case JGUI_Text_VAlignment_Bottom:
		align_offset_y = (owner_size.y * -0.5f) + (owner_line * font_height);
		break;
	}


	for (auto& info : charInfo)
	{
		// offset 
		float xoffset = info.xoffset * font_size_ratio;
		float yoffset = info.yoffset * font_size_ratio;


		// w & h
		float w = info.width  * font_size_ratio;
		float h = info.height * font_size_ratio;
		float hw = w * 0.5f;
		float hh = h * 0.5f;


		// text tex 
		double tleft    = info.x / fontFileInfo.scaleW;
		double ttop     = info.y / fontFileInfo.scaleH;
		double tright   = tleft + (info.width / fontFileInfo.scaleW);
		double tbottom  = ttop + (info.height / fontFileInfo.scaleH);


		JVector2 pos = start_pos;
		pos.x += xoffset;
		pos.y -= yoffset;

		std::vector<JGUIVertex> vv(4);

		vv[0].Position = { pos.x     + align_offset_x, pos.y - h + align_offset_y,  0.0f };
		vv[1].Position = { pos.x     + align_offset_x, pos.y     + align_offset_y,  0.0f };
		vv[2].Position = { pos.x + w + align_offset_x, pos.y     + align_offset_y,  0.0f };
		vv[3].Position = { pos.x + w + align_offset_x, pos.y - h + align_offset_y,  0.0f };


		vv[0].Texcoord = { (float)tleft , (float)tbottom };
		vv[1].Texcoord = { (float)tleft , (float)ttop };
		vv[2].Texcoord = { (float)tright, (float)ttop };
		vv[3].Texcoord = { (float)tright, (float)tbottom };

		for (int i = 0; i < 4; ++i)
		{
			vv[i].Color = owner->GetColor();
		}

		vector<uint32_t> ii(6);

		ii[0] = 0 + ioff; ii[1] = 1 + ioff; ii[2] = 2 + ioff;
		ii[3] = 0 + ioff; ii[4] = 2 + ioff; ii[5] = 3 + ioff;

		ioff += 4;

		v_array.insert(v_array.end(), vv.begin(), vv.end());
		i_array.insert(i_array.end(), ii.begin(), ii.end());


		start_pos.x += (float)info.xadvance * font_size_ratio;
	}
	prev_text = text;
	is_dirty = true;

}
