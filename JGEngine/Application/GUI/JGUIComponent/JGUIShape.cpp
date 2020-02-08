#include "pch.h"
#include "JGUIShape.h"
#include "JGUIPanel.h"
#include "JGUIRectTransform.h"
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
	ChildLock();
}

void JGUIShape::Start()
{

	FindPanel();
}

void JGUIShape::Tick(const JGUITickEvent& e)
{
	if (m_ParentDirty)
	{
		m_ParentDirty = false;
		FindPanel();
	}
	auto transform = GetTransform();

	JVector2 window_size = GetOwnerWindow()->GetTransform()->GetSize();

	if (transform->IsDirty() || 
		(window_size != m_PrevWindowSize))
	{




		m_PrevWindowSize = window_size;
		auto pos = transform->GetPosition();
		pos += m_Offset;
		
		auto angle = transform->GetAngle();
		auto scale = transform->GetScale();
		if (m_IsWindowTexture)
		{
			if (GetOwnerWindow()->GetParent()) pos = GetOwnerWindow()->GetParent()->ConvertToScreenPos(pos);
			else pos = GetOwnerWindow()->ConvertToScreenPos(pos);
		}
		else
		{
			pos = GetOwnerWindow()->ConvertToScreenPos(pos);
		}
		


		auto pivot = transform->GetPivot();
		pivot.x = 1.0f - (pivot.x  + 0.5f );
		
		pivot.y -= 0.5f;

		auto shape_size = transform->GetSize();
		pos.x += (shape_size.x * pivot.x);
		pos.y += (shape_size.y * pivot.y);


		// 매트릭스계산
		auto t = JMatrix::Translation(JVector3(pos.x, pos.y, 0.0f));
		auto r = JMatrix::Rotation(JQuaternion::ToQuaternion({ 0.0f,0.0f, angle }));
		auto s = JMatrix::Scaling(JVector3(scale.x, scale.y, 1.0f));

		auto m = s * r * t;
		if (m_Instance)
		{
			m_Instance->Set("World", JMatrix::Transpose(m));
		}
		transform->Flush();
	}
}
void JGUIShape::Destroy()
{
	DestroyRI();
}
void JGUIShape::ParentUpdateNotification()
{
	m_ParentDirty = true;
}
void JGUIShape::SetActive(bool active)
{
	JGUIComponent::SetActive(active);
	if(m_RenderItem == nullptr)	SetParent(GetParent());
	if (m_RenderItem)
	{
		m_RenderItem->SetActive(active);
	}
}
void JGUIShape::SetParent(JGUIComponent* parent)
{
	JGUIComponent::SetParent(parent);
	if (m_ParentDirty && m_RenderItem == nullptr)
	{
		m_ParentDirty = false;
		FindPanel();
	}
}
void JGUIShape::SetColor(const JColor& color)
{
	m_Color = color;
	if (m_RenderItem == nullptr)	SetParent(GetParent());
	if (m_RenderItem)
	{
		m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", color);
	}
}
void JGUIShape::DestroyRI()
{
	if (m_RenderItem)
	{
		m_Instance = nullptr;
		RE::RenderEngine::DestroyRenderItem(GetOwnerWindow()->GetID(), m_RenderItem);
		m_RenderItem = nullptr;
	}
}
void JGUIShape::FindPanel()
{
	JGUIComponent* p = GetParent();
	bool is_find = false;
	uint64_t cnt = 0;
	while (p != nullptr)
	{
		cnt++;
		if (typeid(*p) == typeid(JGUIPanel))
		{
			m_OwnerPanel = (JGUIPanel*)p;
			is_find = true;

			if (!m_IsWindowTexture)
			{
				if (cnt == 1 && typeid(*this) == typeid(JGUIRectangle))
				{
					if (m_OwnerPanel->GetParent() == nullptr) m_Priority = 0;
					else m_Priority = cnt;
				}
				else  m_Priority = m_OwnerPanel->GetPriority() + cnt;
			}
			else
			{
				m_Priority = GetOwnerWindow()->GetPriority();
			}
			break;
		}
		p = p->GetParent();
	}

	if (!is_find)
	{
		m_OwnerPanel = nullptr;
		DestroyRI();
	}
	else
	{
		if (m_RenderItem == nullptr)
		{
			CreateRI();
		}
		else
		{
			m_RenderItem->SetPriority(m_Priority);
		}
	}
}
void JGUIShape::CreateRI()
{
}

void JGUIWindowTexture::Bind(const std::string& moduleKey, uint64_t parnet_id)
{
	if (m_ParentID == parnet_id && m_RenderItem)
	{
		m_RenderItem->GetMaterial()->SetTexture("Image", moduleKey);

		auto desc = m_RenderItem->GetMaterial()->GetTextureArray()[0].GetDesc();

		return;
	}


	m_ParentID = parnet_id;
	if (m_RenderItem)
	{
		DestroyRI();
	}
	auto transform = GetTransform();
	auto size = transform->GetSize();
	m_RenderItem = RE::RenderEngine::CreateRenderItem(parnet_id,
		RE::ERenderItemUsage::GUI, GetName() + "_RI");

	m_RenderItem->SetMaterial(RE_GUI_OneTextureDefault);
	m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	m_RenderItem->GetMaterial()->SetTexture("Image", moduleKey);
	m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
	m_RenderItem->SetPriority(GetOwnerWindow()->GetPriority());
	m_Instance = m_RenderItem->AddInstance();
	m_RenderItem->SetActive(IsActive());
}

void JGUIWindowTexture::UnBind()
{
	if (m_RenderItem)
	{
		DestroyRI();
	}


	m_ParentID = -1;
}

void JGUIWindowTexture::Awake()
{
	JGUIShape::Awake();
	m_IsWindowTexture = true;
}

void JGUIWindowTexture::Resize(const JGUIResizeEvent& e)
{
	auto size = GetTransform()->GetSize();
	if (m_RenderItem)
	{
		m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
	}
}

void JGUIWindowTexture::DestroyRI()
{
	if (m_RenderItem)
	{
		m_Instance = nullptr;
		RE::RenderEngine::DestroyRenderItem(m_ParentID, m_RenderItem);
		m_RenderItem = nullptr;
	}
}


void JGUIRectangle::SetImage(const std::string& texture_image)
{
	if (m_RenderItem)
	{
		if (m_ImageName.empty()) m_RenderItem->SetMaterial(RE_GUI_OneTextureDefault);
		m_ImageName = texture_image;
		m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
		m_RenderItem->GetMaterial()->SetTexture("Image", texture_image);
	}

}
void JGUIRectangle::FillOn()
{
	m_IsFill = true;
	CreateMesh();
}
void JGUIRectangle::EmptyOn()
{
	m_IsFill = false;
	CreateMesh();
}
void JGUIRectangle::CreateRI()
{
	if (m_RenderItem)
	{
		DestroyRI();
	}
	auto transform = GetTransform();
	auto size = transform->GetSize();

	m_RenderItem = RE::RenderEngine::CreateRenderItem(GetOwnerWindow()->GetID(),
		RE::ERenderItemUsage::GUI, GetName() + "_RI");
	
	if(m_ImageName.empty()) m_RenderItem->SetMaterial(RE_GUI_DefaultMaterial);
	else m_RenderItem->SetMaterial(RE_GUI_OneTextureDefault);

	
	m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	if (!m_ImageName.empty()) m_RenderItem->GetMaterial()->SetTexture("Image", m_ImageName);

	CreateMesh();

	m_RenderItem->SetPriority(m_Priority);
	m_Instance = m_RenderItem->AddInstance();
	m_RenderItem->SetActive(IsActive());

}
void JGUIRectangle::Resize(const JGUIResizeEvent& e)
{
	if (m_RenderItem == nullptr)	SetParent(GetParent());
	if (m_RenderItem)
	{
		CreateMesh();
	}
}
void JGUIRectangle::CreateMesh()
{
	if (m_RenderItem == nullptr) return;
	auto size = GetTransform()->GetSize();
	if (m_IsFill)
	{
		m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
	}
	else
	{
		m_RenderItem->SetMesh(RE::ReGuiMesh::CreateEmptyRect(size.x, size.y, m_Thick));
	}

}
void JGUIText::SetFont(const std::string& str)
{
	m_FontName = str;
	SetText(m_Text);
}

void JGUIText::SetText(const std::string& str)
{
	if (m_RenderItem == nullptr) CreateRI();
	if (str.empty()) return;
	if (m_RenderItem == nullptr) return;
	auto dmesh = m_RenderItem->GetMesh();
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

	auto dmesh = m_RenderItem->GetMesh();
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
	auto dmesh = m_RenderItem->GetMesh();
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

JVector2 JGUIText::GetTextLastPos()
{
	if (m_Text.length() == 0) return JVector2();



	string last = m_Text.substr(m_Text.length() - 1, m_Text.length());
	auto  charInfo = JGUI::GetJGUIFontManager()->GetFontCharInfo(m_FontName,s2ws(last));
	auto  fontfileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);

	auto pos = GetTextPos(m_Text.length() - 1);
	float font_size_ratio = m_FontSize / (float)fontfileInfo.default_font_size;

	auto info = *(charInfo.begin() );
	pos.x += ((float)info.xadvance * font_size_ratio);
	return pos;
}

const JVector2& JGUIText::GetTextPos(int n) const
{
	return m_TextPosMap[n];
}

void JGUIText::CreateRI()
{
	if (m_RenderItem)
	{
		DestroyRI();
	}
	if (m_FontName == "")
	{
		m_FontName = JGUI::GetDefaultFontName();
	}
	auto transform = GetTransform();
	auto size = transform->GetSize();
	auto fileInfo = JGUI::GetJGUIFontManager()->GetFontFileInfo(m_FontName);


	m_RenderItem = RE::RenderEngine::CreateRenderItem(GetOwnerWindow()->GetID(),
		RE::ERenderItemUsage::GUI, GetName() + "_RI");
	if (m_RenderItem == nullptr) return;
	m_RenderItem->SetMaterial(RE_GUI_TextMaterial);
	m_RenderItem->SetMesh(make_shared<RE::ReGuiMesh>());
	m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	m_RenderItem->GetMaterial()->SetTexture("Image", fileInfo.page[0]);

	m_RenderItem->SetPriority(m_Priority);
	m_Instance = m_RenderItem->AddInstance();
	m_RenderItem->SetActive(IsActive());
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
		info.width  += 1;
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
	m_Offset.x = -(text_rect_size.x / 2.0f);
	m_Offset.y = -(lineCount - 1) * font_height * 0.5f;
}

