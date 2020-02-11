#include "pch.h"
#include "JGUIShape.h"
#include "JGUIRectTransform.h"
#include "JGUIRenderItem.h"
#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIFont/JGUIFontManager.h"

#include "RenderEngine.h"
#include "Object/ReObject/ReMaterial.h"
#include "Object/ReObject/RenderItem.h"
#include "Object/ReObject/ReMesh.h"
#include "Object/DxObject/Resource.h"
#include "Object/Shader/ShaderModule.h"

using namespace std;

void JGUIShape::Awake()
{
	m_RI = JGUI::CreateJGUIComponent<JGUIRenderItem>("RenderItem", GetOwnerWindow(),
		JGUI_ComponentFlag_NoChild | JGUI_ComponentFlag_NoParent | JGUI_ComponentFlag_LockCreateFunction);
	m_RI->SetDrawingWindow(GetOwnerWindow());
	m_RI->SetRIPriority(GetRISortingOrder());

	InitRenderItem(m_RI->GetRI());
	GetTransform()->AttachTransform(m_RI->GetTransform());
}

void JGUIShape::Destroy()
{

	JGUI::DestroyObject(m_RI);
}

void JGUIShape::Tick(const JGUITickEvent& e)
{
	m_RI->SetRIPriority(GetRISortingOrder());
	m_RI->RenderUpdate();
}

void JGUIShape::SetActive(bool is_active)
{
	m_RI->SetActive(is_active);
	JGUIComponent::SetActive(is_active);
}

void JGUIShape::CreateRI(JGUIWindow* drawing_win)
{
	m_RI->SetRIPriority(GetRISortingOrder());
	m_RI->SetDrawingWindow(drawing_win);
	InitRenderItem(m_RI->GetRI());
}


void JGUIWindowTexture::Bind(const std::string& moduleKey)
{
	is_code = true;
	m_ModuleKey = moduleKey;
	CreateRI(GetOwnerWindow()->GetParent());
}
void JGUIWindowTexture::UnBind()
{
	GetRenderItem()->SetDrawingWindow(nullptr);
}

void JGUIWindowTexture::InitRenderItem(RE::RenderItem* ri)
{
	if (m_ModuleKey.empty()) return;
	auto size = GetTransform()->GetSize();
	ri->SetMaterial(RE_GUI_OneTextureDefault);
	ri->GetMaterial()->SetValueAsFloat4("Color", JColor(1,1,1,1));
	ri->GetMaterial()->SetTexture("Image", m_ModuleKey);
	ri->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
}
void JGUIWindowTexture::Tick(const JGUITickEvent& e)
{
	GetRenderItem()->SetRIPriority(GetOwnerWindow()->GetPriority());
	GetRenderItem()->RenderUpdate();
}
void JGUIWindowTexture::Resize(const JGUIResizeEvent& e)
{
	auto size = GetTransform()->GetSize();
	if (GetRenderItem()->GetRI())
	{
		GetRenderItem()->GetRI()->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
	}

}



void JGUIImage::InitRenderItem(RE::RenderItem* ri)
{
	auto size = GetTransform()->GetSize();
	if (m_ImageSource == "none")
	{
		ri->SetMaterial(RE_GUI_DefaultMaterial);
		ri->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	}
	else
	{
		ri->SetMaterial(RE_GUI_OneTextureDefault);
		ri->GetMaterial()->SetValueAsFloat4("Color", m_Color);
		ri->GetMaterial()->SetTexture("Image", m_ImageSource);
	
	}
	ri->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
}

void JGUIImage::Resize(const JGUIResizeEvent& e)
{
	auto size = GetTransform()->GetSize();
	if (GetRenderItem()->GetRI())
	{
		GetRenderItem()->GetRI()->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
	}
}

void JGUIImage::SetImage(const std::string& path)
{
	m_ImageSource = path;
	if (GetRenderItem()->GetRI())
	{
		CreateRI(GetOwnerWindow());
	}
}

void JGUIImage::SetColor(const JColor& color)
{
	m_Color = color;
	if (GetRenderItem()->GetRI())
	{
		GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	}
}

void JGUIImage::SetColor(float r, float g, float b, float a)
{
	SetColor(JColor(r, g, b, a));
}









void JGUIRectangle::InitRenderItem(RE::RenderItem* ri)
{
	auto size = GetTransform()->GetSize();
	ri->SetMaterial(RE_GUI_DefaultMaterial);
	ri->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	ri->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
}
void JGUIRectangle::Resize(const JGUIResizeEvent& e)
{
	if (GetRenderItem()->GetRI())
	{
		GetRenderItem()->GetRI()->SetMesh(RE::ReGuiMesh::CreateFillRect(e.width, e.height));
	}

}
void JGUIRectangle::SetColor(const JColor& color)
{
	m_Color = color;
	if (GetRenderItem()) GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	
}
void JGUIRectangle::SetColor(float r, float g, float b, float a)
{
	m_Color = JColor(r, g, b, a);
	if (GetRenderItem()) GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat4("Color", m_Color);
}





void JGUIEmptyRectangle::InitRenderItem(RE::RenderItem* ri)
{
	auto size = GetTransform()->GetSize();
	ri->SetMesh(RE::ReGuiMesh::CreateEmptyRect(size.x, size.y, m_Thick));
	ri->SetMaterial(RE_GUI_DefaultMaterial);
	ri->GetMaterial()->SetValueAsFloat4("Color", m_Color);
}
void JGUIEmptyRectangle::Resize(const JGUIResizeEvent& e)
{
	if (GetRenderItem()->GetRI())
	{
		GetRenderItem()->GetRI()->SetMesh(RE::ReGuiMesh::CreateEmptyRect(e.width, e.height, m_Thick));
	}
	
}
void JGUIEmptyRectangle::SetColor(const JColor& color)
{
	m_Color = color;
	if (GetRenderItem()) GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat4("Color", m_Color);

}
void JGUIEmptyRectangle::SetColor(float r, float g, float b, float a)
{
	m_Color = JColor(r, g, b, a);
	if (GetRenderItem()) GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat4("Color", m_Color);
}

void JGUIEmptyRectangle::SetThickness(float thick)
{
	if (m_Thick != thick)
	{
		auto size = GetTransform()->GetSize();
		GetRenderItem()->GetRI()->SetMesh(RE::ReGuiMesh::CreateEmptyRect(size.x, size.y, m_Thick));
	}
	m_Thick = thick;
}










void JGUIText::InitRenderItem(RE::RenderItem* ri)
{
	auto size = GetTransform()->GetSize();
	auto fileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);


	ri->SetMaterial(RE_GUI_TextMaterial);
	ri->SetMesh(make_shared<RE::ReGuiMesh>());
	ri->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	ri->GetMaterial()->SetTexture("Image", fileInfo.page[0]);
}


void JGUIText::SetFont(const std::string& str)
{
	m_FontName = str;
	SetText(m_Text);
}

void JGUIText::SetText(const std::string& str)
{
	if (str.empty()) return;
	auto dmesh = GetRenderItem()->GetRI()->GetMesh();
	if (dmesh->GetType() != RE::EReMeshType::GUI) return;
	dmesh->Reset();
	// 
	m_Text = str;
	m_TextPosMap.clear();
	//
	UpdateTextSize(str);
	// 
	auto mesh = (RE::ReGuiMesh*)dmesh;
	m_IOffset = 0;
	m_StartPos = { 0,0 };
	std::vector<JGUIVertex> v;
	std::vector<uint32_t> i;
	std::vector<JVector2> pos;
	auto _pair = ConvertVertex(m_IOffset, m_StartPos, m_Text, v, i, pos);
	m_TextPosMap.insert(m_TextPosMap.end(), pos.begin(), pos.end());
	m_IOffset = _pair.first;
	m_StartPos = _pair.second;
	mesh->Create(v, i);
}

void JGUIText::AddText(const std::string& str)
{
	if (m_Text.empty())
	{
		SetText(str); return;
	}

	auto dmesh = GetRenderItem()->GetRI()->GetMesh();
	if (dmesh->GetType() != RE::EReMeshType::GUI) return;
	// 
	m_Text += str;
	UpdateTextSize(m_Text);
	auto mesh = (RE::ReGuiMesh*)dmesh;
	std::vector<JGUIVertex> v;
	std::vector<uint32_t> i;
	std::vector<JVector2> p;
	auto _pair = ConvertVertex(m_IOffset, m_StartPos, str, v, i, p);
	m_TextPosMap.insert(m_TextPosMap.end(), p.begin(), p.end());
	m_IOffset = _pair.first;
	m_StartPos = _pair.second;
	mesh->Add(v, i);
}

void JGUIText::Insert(int idx, const std::string& str)
{
	if (idx > m_Text.length()) return;
	if (idx == m_Text.length())
	{
		AddText(str); return;
	}

	uint32_t len = (uint32_t)m_Text.length();
	string sub = m_Text.substr(idx, m_Text.length());
	RemoveRange(idx, len);
	AddText(str + sub);
}

void JGUIText::Modify(int idx, const std::string& str)
{
	//
	uint32_t origin_len = (uint32_t)m_Text.length();
	uint32_t len = (uint32_t)str.length();
	if (len + idx > origin_len) return;

	RemoveRange(idx, idx + len);
	Insert(idx, str);
}

void JGUIText::RemoveFirstOf(const std::string& str)
{

}
void JGUIText::RemoveLastOf(const std::string& str)
{

}
void JGUIText::RemoveAll(const std::string& str)
{

}

void JGUIText::RemoveRange(int start_pos, int end_pos)
{
	if (m_Text.empty()) return;
	if (start_pos >= end_pos) return;
	auto dmesh = GetRenderItem()->GetRI()->GetMesh();
	if (dmesh->GetType() != RE::EReMeshType::GUI) return;
	auto mesh = (RE::ReGuiMesh*)dmesh;

	m_StartPos = m_TextPosMap[start_pos];
	m_IOffset = start_pos * m_VertexCount;




	uint32_t vstart = start_pos * m_VertexCount;
	uint32_t istart = start_pos * m_IndexCount;
	uint32_t vend = end_pos * m_VertexCount;
	uint32_t iend = end_pos * m_IndexCount;
	m_Text.erase(start_pos, end_pos - start_pos);
	m_TextPosMap.erase(m_TextPosMap.begin() + start_pos, m_TextPosMap.begin() + end_pos);
	mesh->Remove(vstart, vend, istart, iend);

	string sub = m_Text.substr(start_pos, m_Text.length());
	if (!sub.empty())
	{
		vector<JGUIVertex> v;
		vector<uint32_t> i;
		vector<JVector2> p;
		auto info_pair = ConvertVertex(m_IOffset, m_StartPos, sub, v, i, p);
		m_IOffset = info_pair.first;
		m_StartPos = info_pair.second;
		for (uint32_t i = start_pos; i < (uint32_t)m_TextPosMap.size(); ++i)
		{
			m_TextPosMap[i] = p[i - start_pos];
		}
		mesh->Modify(v, vstart, i, istart);
	}
	UpdateTextSize(m_Text);
}

void JGUIText::RemoveBack()
{
	RemoveRange((int)m_Text.size() - 1, (int)m_Text.size());
}

uint32_t JGUIText::Length() const
{
	return (uint32_t)m_Text.length();
}

JGUIText& JGUIText::operator=(const std::string& str)
{
	SetText(str);
	return *this;
}
JGUIText& JGUIText::operator+=(const std::string& str)
{
	AddText(str);
	return *this;
}

void JGUIText::SetFontSize(float size)
{
	m_FontSize = size;
	SetText(m_Text);
}

void JGUIText::SetTextRect(float width, float height)
{
	m_TextRect.width = width;
	m_TextRect.height = height;
	m_TextRect.right = width;
	m_TextRect.bottom = height;
	SetText(m_Text);
}
void JGUIText::SetColor(const JColor& color)
{
	m_Color = color;
	if (GetRenderItem())
	{
		GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	}
}
void JGUIText::SetColor(float r, float g, float b, float a)
{
	m_Color = JColor(r, g, b, a);
	if (GetRenderItem())
	{
		GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	}
}
JVector2 JGUIText::GetTextLastPos(int n)
{
	if (m_Text.length() == 0) return JVector2();
	if (m_Text.length() <= n) return JVector2();


	string last = m_Text.substr(n, n + 1);
	auto  charInfo = JGUI::GetJGUIFontManager()->GetFontCharInfo(m_FontName, s2ws(last));
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);

	auto pos = GetTextPos(n);
	float font_size_ratio = m_FontSize / (float)fontfileInfo.default_font_size;

	auto info = *(charInfo.begin());
	pos.x += ((float)info.xadvance * font_size_ratio);
	return pos;
}

const JVector2& JGUIText::GetTextPos(int n) const
{
	return m_TextPosMap[n];
}



std::pair<uint32_t, JVector2> JGUIText::ConvertVertex(
	uint32_t ioffset, JVector2 start_pos, const std::string& str,
	std::vector<JGUIVertex>& v, std::vector<uint32_t>& i, std::vector<JVector2>& p)
{
	auto fontManager = JGUI::GetJGUIFontManager();

	std::wstring wstr = s2ws(str);

	// 각종 폰트 정보들 불러오기
	auto  charInfo = fontManager->GetFontCharInfo(m_FontName, wstr);
	auto  fontfileInfo = fontManager->GetFontFileInfo(m_FontName);
	float font_size_ratio = m_FontSize / (float)fontfileInfo.default_font_size;
	float font_height = (float)fontfileInfo.lineHeight * font_size_ratio;

	// a3. 각 텍스트에 맞는 Vertex생성
	for (auto& info : charInfo)
	{
		info.height += 1;
		info.width += 1;
		// offset
		float xoffset = (float)info.xoffset * font_size_ratio;
		float yoffset = (float)info.yoffset * font_size_ratio;

		// half font text size 
		float w = (float)info.width * font_size_ratio;
		float hh = (float)info.height * 0.5f * font_size_ratio;
		// tex 위치 찾기
		float tleft = (float)info.x / fontfileInfo.scaleW;
		float ttop = (float)info.y / fontfileInfo.scaleH;
		float tright = tleft + ((float)info.width / fontfileInfo.scaleW);
		float tbottom = ttop + ((float)info.height / fontfileInfo.scaleH);

		// offset 처리
		JVector2 pos = start_pos;
		p.push_back(pos);

		pos.x += xoffset;
		pos.y -= (yoffset * 0.5f);

		//  주어진 데이터로 Vertex 생성
		vector<JGUIVertex> vv(m_VertexCount);
		vv[0].Position = { pos.x , pos.y - hh,  0.0f };
		vv[1].Position = { pos.x , pos.y + hh,  0.0f };
		vv[2].Position = { pos.x + w , pos.y + hh,  0.0f };
		vv[3].Position = { pos.x + w , pos.y - hh,  0.0f };

		vv[0].Texcoord = { tleft , tbottom };
		vv[1].Texcoord = { tleft , ttop };
		vv[2].Texcoord = { tright, ttop };
		vv[3].Texcoord = { tright, tbottom };

		for (int i = 0; i < m_VertexCount; ++i)
		{
			vv[i].Color = JColor(1.0f, 1.0f, 1.0f, 1.0f);
		}

		vector<uint32_t> ii(m_IndexCount);
		ii[0] = 0 + ioffset; ii[1] = 1 + ioffset; ii[2] = 2 + ioffset;
		ii[3] = 0 + ioffset; ii[4] = 2 + ioffset; ii[5] = 3 + ioffset;

		ioffset += (int)vv.size();
		if (start_pos.x + (float)m_FontSize > m_TextRect.width)
		{
			start_pos.x = 0.0f;
			start_pos.y -= font_height;
		}
		else
		{
			start_pos.x += (float)info.xadvance * font_size_ratio;
		}


		v.insert(v.end(), vv.begin(), vv.end());
		i.insert(i.end(), ii.begin(), ii.end());
	}
	return pair<uint32_t, JVector2>(ioffset, start_pos);
}

void JGUIText::UpdateTextSize(const std::string& str)
{
	std::wstring wstr = s2ws(str);


	auto fontManager = JGUI::GetJGUIFontManager();
	auto charInfo = fontManager->GetFontCharInfo(m_FontName, wstr);
	auto fontfileInfo = fontManager->GetFontFileInfo(m_FontName);
	float font_size_ratio = m_FontSize / (float)fontfileInfo.default_font_size;
	float font_height = fontfileInfo.lineHeight * font_size_ratio;
	// a1. 텍스트 전체 크기를 구한다.
	JVector2 text_rect_size;

	float accX = 0.0f;
	int   lineCount = 0;
	for (auto& info : charInfo)
	{
		accX += (info.xadvance * font_size_ratio);
	}
	lineCount = (int)(accX / m_TextRect.width) + 1;
	if (lineCount == 1) text_rect_size.x = accX;
	else text_rect_size.x = m_TextRect.width;
	text_rect_size.y = font_height * lineCount;

	GetTransform()->SetSize(text_rect_size);

	auto offset = GetRenderItem()->GetOffset();
	offset.x = -(text_rect_size.x / 2.0f);
	offset.y = -(lineCount - 1) * font_height * 0.5f;
	GetRenderItem()->SetOffset(offset);

	auto window_size = GetOwnerWindow()->GetTransform()->GetSize();
	float half_window_width = window_size.x * 0.5f;
	JVector2 ClipRange = { 0.0f, text_rect_size.x };
	auto pos = GetTransform()->GetPosition();
	ClipRange.x += (pos.x - half_window_width);
	ClipRange.y += (pos.x - half_window_width);



    bool result = GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat2("Clip", ClipRange);

}


