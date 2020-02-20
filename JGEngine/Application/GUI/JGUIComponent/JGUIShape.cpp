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
	if (GetRISortingOrder()== 5)
	{
		auto  name = GetName();
	}
	m_RI->SetRIPriority(GetRISortingOrder());
	m_RI->RenderUpdate();
}

void JGUIShape::SetActive(bool is_active)
{
	m_RI->SetActive(is_active);
	JGUIComponent::SetActive(is_active);
}

void JGUIShape::SetClipFlag(EJGUI_Clip_Flags flags)
{
	int flag = RE_GUI_CLIP_NONE;
	if (flags & JGUI_Clip_Flag_X)
	{
		flag |= RE_GUI_CLIP_XON;
	}
	if (flags & JGUI_Clip_Flag_Y)
	{
		flag |= RE_GUI_CLIP_YON;
	}
	if (flags & JGUI_Clip_Flag_Reverse)
	{
		flag |= RE_GUI_CLIP_REVERSE;
	}
	if (m_RI && m_RI->GetRI() && m_RI->GetRI()->GetMaterial())
	{
		m_RI->GetRI()->GetMaterial()->SetValueAsInt("Clip", flag);
	}
	m_ClipFlag = flags;
}

void JGUIShape::CreateRI(JGUIWindow* drawing_win)
{
	m_RI->SetRIPriority(GetRISortingOrder());
	m_RI->SetDrawingWindow(drawing_win);
	InitRenderItem(m_RI->GetRI());
	SetClipFlag(m_ClipFlag);
}


void JGUIWindowTexture::Bind(const std::string& moduleKey)
{
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
	ri->GetMaterial()->SetValueAsFloat4("Color", JColor(1, 1, 1, 1));
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










void JGUITextMesh::Awake()
{
	JGUIShape::Awake();
	UpdateTextFlags();
}

void JGUITextMesh::InitRenderItem(RE::RenderItem* ri)
{
	auto size = GetTransform()->GetSize();
	auto fileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);


	ri->SetMaterial(RE_GUI_TextMaterial);
	ri->SetMesh(make_shared<RE::ReGuiMesh>());
	ri->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	ri->GetMaterial()->SetTexture("Image", fileInfo.page[0]);
}


void JGUITextMesh::SetFont(const std::string& str)
{
	m_FontName = str;
	SetText(m_Text);
}

void JGUITextMesh::SetText(const std::string& str)
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

void JGUITextMesh::AddText(const std::string& str)
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

void JGUITextMesh::Insert(int idx, const std::string& str)
{
	if (idx > m_Text.length()) return;
	if (idx == m_Text.length())
	{
		AddText(str); return;
	}

	bool is_over = false;
	if (m_DrawingType == JGUI_Text_Drawing_RightPushed)
	{
		float last_pos_x = m_TextPosMap[m_Text.length() - 1].x + GetTextWidth(m_Text.substr(m_Text.length() - 1));
		if (last_pos_x >= m_TextRect.right)
		{
			is_over = true;
		}
	}


	uint32_t len = (uint32_t)m_Text.length();
	string sub = m_Text.substr(idx, m_Text.length());

	float temp = m_TextWidthDelta;
	RemoveRange(idx, len);
	m_TextWidthDelta = temp;



	AddText(str + sub);


	// 여기서 넘어서는 순간 PushLeft한다. 이거 고치셈
	if (m_DrawingType == JGUI_Text_Drawing_RightPushed)
	{
		if (GetTextLastPos(idx).x >= m_TextRect.right)
		{
			PushRight(GetTextLastPos(idx).x - m_TextRect.right);
		}
		else
		{
			float text_width = GetTextWidth(m_Text);
			float str_width = GetTextWidth(str);

			if (text_width >= m_TextRect.right)
			{
				float last_pos_x = m_TextPosMap[m_Text.length() - 1].x + GetTextWidth(m_Text.substr(m_Text.length() - 1));
				if (last_pos_x >= m_TextRect.right && is_over == false)
				{
					PushLeft(text_width - m_TextRect.right);
				}
				else
				{
					PushLeft(str_width);
				}
			
			}
		}
		UpdateTextSize(m_Text);
	}
}

void JGUITextMesh::Modify(int idx, const std::string& str)
{
	uint32_t origin_len = (uint32_t)m_Text.length();
	uint32_t len = (uint32_t)str.length();
	if (len + idx > origin_len) return;
	float temp = m_TextWidthDelta;
	RemoveRange(idx, idx + len);
	m_TextWidthDelta = temp;
	Insert(idx, str);
}

void JGUITextMesh::RemoveFirstOf(const std::string& str)
{

}
void JGUITextMesh::RemoveLastOf(const std::string& str)
{

}
void JGUITextMesh::RemoveAll(const std::string& str)
{

}

void JGUITextMesh::RemoveRange(int start_pos, int end_pos)
{
	if (m_Text.empty()) return;
	if (start_pos >= end_pos) return;
	auto dmesh = GetRenderItem()->GetRI()->GetMesh();
	if (dmesh->GetType() != RE::EReMeshType::GUI) return;
	auto mesh = (RE::ReGuiMesh*)dmesh;

	m_StartPos = m_TextPosMap[start_pos];
	m_IOffset = start_pos * m_VertexCount;



	{
		m_TextWidthDelta -= GetTextWidth(m_Text.substr(start_pos, end_pos - start_pos));
		m_TextWidthDelta = std::max<float>(m_TextWidthDelta, 0.0f);
	}





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

void JGUITextMesh::RemoveBack()
{
	RemoveRange((int)m_Text.size() - 1, (int)m_Text.size());
}

uint32_t JGUITextMesh::Length() const
{
	return (uint32_t)m_Text.length();
}

JGUITextMesh& JGUITextMesh::operator=(const std::string& str)
{
	SetText(str);
	return *this;
}
JGUITextMesh& JGUITextMesh::operator+=(const std::string& str)
{
	AddText(str);
	return *this;
}

void JGUITextMesh::SetFontSize(float size)
{
	float ratio = size / m_FontSize;

	m_FontSize = size;
	m_TextWidthDelta = m_TextWidthDelta * ratio;
	SetText(m_Text);
}

void JGUITextMesh::SetTextRect(float width, float height)
{
	m_TextRect.left = 0.0f;
	m_TextRect.right = 0.0f;
	m_TextRect.right = width;
	m_TextRect.bottom = height;
	SetText(m_Text);
}
void JGUITextMesh::SetColor(const JColor& color)
{
	m_Color = color;
	if (GetRenderItem())
	{
		GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	}
}
void JGUITextMesh::SetColor(float r, float g, float b, float a)
{
	m_Color = JColor(r, g, b, a);
	if (GetRenderItem())
	{
		GetRenderItem()->GetRI()->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	}
}
JVector2 JGUITextMesh::GetTextLastPos(int n)
{
	if (m_Text.length() == 0) return JVector2();
	if (m_Text.length() <= n) return JVector2();
	if (n < 0) return JVector2();

	string last = m_Text.substr(n, n + 1);
	auto  charInfo = JGUI::GetJGUIFontManager()->GetFontCharInfo(m_FontName, s2ws(last));
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);

	auto pos = m_TextPosMap[n];
	float font_size_ratio = m_FontSize / (float)fontfileInfo.default_font_size;

	auto info = *(charInfo.begin());
	pos.x += ((float)info.xadvance * font_size_ratio);

	float font_height = (float)fontfileInfo.lineHeight * font_size_ratio;
	switch (m_DrawingType)
	{
	case JGUI_Text_Drawing_Discard:
	case JGUI_Text_Drawing_Ignore:
	case JGUI_Text_Drawing_NextLine:
		break;
	case JGUI_Text_Drawing_RightPushed:
		
		if (m_TextWidth >= m_TextRect.width())
		{
			pos.x -= (m_TextWidth - m_TextRect.width());
			pos.x += m_TextWidthDelta;
		}
		break;
	case JGUI_Text_Drawing_DownPushed:
		if (m_LineCount > 1)
		{
			pos.y += m_LineCount * font_height;
		}
		break;
	}
	return pos;
}

JVector2 JGUITextMesh::GetTextPos(int n) 
{
	if (m_Text.length() == 0) return JVector2();
	if (m_Text.length() <= n) return JVector2();
	if (n < 0) return JVector2();

	auto pos = m_TextPosMap[n];
	string last = m_Text.substr(n, n + 1);
	auto  charInfo = JGUI::GetJGUIFontManager()->GetFontCharInfo(m_FontName, s2ws(last));
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);
	float font_size_ratio = m_FontSize / (float)fontfileInfo.default_font_size;
	float font_height = (float)fontfileInfo.lineHeight * font_size_ratio;
	switch (m_DrawingType)
	{
	case JGUI_Text_Drawing_Discard:
	case JGUI_Text_Drawing_Ignore:
	case JGUI_Text_Drawing_NextLine:
		break;
	case JGUI_Text_Drawing_RightPushed:
		if (m_TextWidth >= m_TextRect.width())
		{
			pos.x -= (m_TextWidth - m_TextRect.width());
			pos.x += m_TextWidthDelta;
		}
		// 살짝 조정 
		break;
	case JGUI_Text_Drawing_DownPushed:
		if (m_LineCount > 1)
		{
			pos.y += m_LineCount * font_height;
		}
		// 살짝 조정
		break;
	}
	return pos;
}
float JGUITextMesh::GetTextWidth() const
{
	return GetTextWidth(m_Text);
}
void JGUITextMesh::PushRight(int last_index_located)
{
	if (m_DrawingType != JGUI_Text_Drawing_RightPushed) return;
	if (last_index_located < 0) return;

	auto text_pos = GetTextLastPos(last_index_located);

	if (text_pos.x <= m_TextRect.width()) return;

	float delta = text_pos.x - m_TextRect.width();


	PushRight(delta);
	// index에 해당하는 위치와 현재 위치의 차이만큼 offset 조정
}
void JGUITextMesh::PushRight(float off)
{
	auto clip = GetRenderItem()->GetClipRect();
	auto offset = GetRenderItem()->GetOffset();
	m_TextWidthDelta -= off;
	// 앤 델타만큼 빼져야되고

	clip.left += off; clip.right += off;
	offset.x -= off;

	GetRenderItem()->SetClipRect(clip);
	GetRenderItem()->SetOffset(offset);
	GetRenderItem()->GetTransform()->SendDirty();
}

void JGUITextMesh::PushLeft(int start_index_located)
{
	if (m_DrawingType != JGUI_Text_Drawing_RightPushed) return;
	if (start_index_located < 0) return;

	auto text_pos = GetTextPos(start_index_located);

	
	if (text_pos.x >= 0) return;

	float delta = -text_pos.x;
	PushLeft(delta);
}
void JGUITextMesh::PushLeft(float off)
{
	auto clip = GetRenderItem()->GetClipRect();
	auto offset = GetRenderItem()->GetOffset();
	m_TextWidthDelta += off;


	clip.left -= off; clip.right -= off;
	offset.x += off;

	GetRenderItem()->SetClipRect(clip);
	GetRenderItem()->SetOffset(offset);
	GetRenderItem()->GetTransform()->SendDirty();
}
void JGUITextMesh::PushUp(int start_line_located)
{
	if (m_DrawingType != JGUI_Text_Drawing_DownPushed) return;
}
void JGUITextMesh::PushDown(int end_line_located)
{
	if (m_DrawingType != JGUI_Text_Drawing_DownPushed) return;
}
void JGUITextMesh::SetTextFlags(EJGUI_Text_Flags flags)
{
	m_TextFlags = flags;
	UpdateTextFlags();
}
void JGUITextMesh::SetTextDrawing(EJGUI_Text_Drawing type)
{
	m_DrawingType = type;
	SetText(m_Text);
}
std::pair<uint32_t, JVector2> JGUITextMesh::ConvertVertex(
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

		if (m_DrawingType == JGUI_Text_Drawing_NextLine ||
			m_DrawingType == JGUI_Text_Drawing_DownPushed)
		{
			if (start_pos.x + (float)m_FontSize > m_TextRect.width())
			{
				start_pos.x = 0.0f;
				start_pos.y -= font_height;
			}
			else
			{
				start_pos.x += (float)info.xadvance * font_size_ratio;
			}
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

void JGUITextMesh::UpdateTextSize(const std::string& str)
{
	std::wstring wstr = s2ws(str);
	SetClipFlag(JGUI_Clip_Flag_None);

	auto fontManager = JGUI::GetJGUIFontManager();
	auto charInfo = fontManager->GetFontCharInfo(m_FontName, wstr);
	auto fontfileInfo = fontManager->GetFontFileInfo(m_FontName);
	float font_size_ratio = m_FontSize / (float)fontfileInfo.default_font_size;
	float font_height = fontfileInfo.lineHeight * font_size_ratio;

	JVector2 offset;
	JVector2 text_rect_size;
	if (m_DrawingType == JGUI_Text_Drawing_NextLine || m_DrawingType == JGUI_Text_Drawing_DownPushed)
	{
		m_TextWidth = 0.0f;
		for (auto& info : charInfo)
		{
			m_TextWidth += (info.xadvance * font_size_ratio);
		}
		m_LineCount = (int)(m_TextWidth / m_TextRect.width()) + 1;
		if (m_LineCount == 1) text_rect_size.x = m_TextWidth;
		else text_rect_size.x = m_TextRect.width();
		text_rect_size.y = font_height * m_LineCount;


		switch (m_DrawingType)
		{
		case JGUI_Text_Drawing_NextLine:
			offset.x = -(text_rect_size.x / 2.0f);
			offset.y = -(m_LineCount - 1) * font_height * 0.5f;

			break;
		case JGUI_Text_Drawing_DownPushed:
			SetClipFlag(JGUI_Clip_Flag_Y);
			offset.x = -(text_rect_size.x / 2.0f);
			offset.y = -(m_LineCount - 1) * font_height * 1.5f;
	
			
			JGUIRect clip_rect;
			clip_rect.top    = -(font_height * 0.5f) + ((m_LineCount - 1) * font_height);
			clip_rect.bottom = font_height * 0.5f + ((m_LineCount - 1) * font_height);
			
			GetRenderItem()->SetClipRect(clip_rect);

			break;
		}


	}
	if (m_DrawingType == JGUI_Text_Drawing_Ignore || m_DrawingType == JGUI_Text_Drawing_Discard ||
		m_DrawingType == JGUI_Text_Drawing_RightPushed)
	{
		m_LineCount = 1;


		JGUIRect clip_rect;
		m_TextWidth = 0.0f;
		for (auto& info : charInfo)
		{
			m_TextWidth += (info.xadvance * font_size_ratio);
		}


		switch (m_DrawingType)
		{
		case JGUI_Text_Drawing_Discard:
			SetClipFlag(JGUI_Clip_Flag_X);

			clip_rect.left = 0.0f;
			clip_rect.right = m_TextRect.width();

			if (m_TextWidth >= m_TextRect.width())
			{
				text_rect_size.x = m_TextRect.width();
				text_rect_size.y = font_height;
			}
			else
			{
				text_rect_size.x = m_TextWidth;
				text_rect_size.y = font_height;
			}

			offset.x = -(text_rect_size.x / 2.0f);
			GetRenderItem()->SetClipRect(clip_rect);
			break;
		case JGUI_Text_Drawing_Ignore:
			text_rect_size.x = m_TextWidth;
			text_rect_size.y = font_height;
			offset.x = -(text_rect_size.x / 2.0f);
			break;
		case JGUI_Text_Drawing_RightPushed:
			SetClipFlag(JGUI_Clip_Flag_X);
	
			if (m_TextWidth >= m_TextRect.width())
			{
				float delta_width = (m_TextWidth - m_TextRect.width());
				clip_rect.left  = delta_width - m_TextWidthDelta;
				clip_rect.right = m_TextRect.width() + delta_width - m_TextWidthDelta;
				text_rect_size.x = m_TextWidth;
				text_rect_size.y = font_height;
				offset.x = -(m_TextWidth / 2.0f) ;
				offset.x -= (m_TextWidth - m_TextRect.width());
				offset.x += m_TextWidthDelta;
			}
			else
			{
				clip_rect.left   = 0.0f;
				clip_rect.right  = m_TextRect.width();
				text_rect_size.x = m_TextWidth;
				text_rect_size.y = font_height;
				offset.x = -(text_rect_size.x / 2.0f);
	
			}
			GetRenderItem()->SetClipRect(clip_rect);
			break;
		}

	}

	GetTransform()->SetSize(text_rect_size);
	GetRenderItem()->SetOffset(offset);
	UpdateTextFlags();
}

void JGUITextMesh::UpdateTextFlags()
{
	if (m_TextFlags & JGUI_Text_Flag_Border)
	{
		if (m_Border == nullptr)
		{
			m_Border = CreateJGUIComponent<JGUIEmptyRectangle>("EmptyRectangle");
		}
		m_Border->SetThickness(1.0f);
		m_Border->SetColor(JColor(0.0f, 0.0f, 0.0f, 1.0f));
		m_Border->GetTransform()->SetSize(m_TextRect.width(), m_TextRect.height());
	}
	else
	{
		if (m_Border)
		{
			DestroyJGUIComponent(m_Border);
			m_Border = nullptr;
		}
	}
}

float JGUITextMesh::GetTextWidth(const std::string& str) const
{
	auto  charInfo = JGUI::GetJGUIFontManager()->GetFontCharInfo(m_FontName,
		s2ws(str));
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);
	float font_size_ratio = m_FontSize / (float)fontfileInfo.default_font_size;
	float accX = 0.0f;
	for (auto& info : charInfo)
	{
		accX += (info.xadvance * font_size_ratio);
	}
	return accX;
}


