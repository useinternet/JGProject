#include "pch.h"
#include "JGUIShape.h"
#include "JGUIPanel.h"
#include "JGUIRectTransform.h"
#include "GUI/JGUIObject/JGUIWindow.h"


#include "RenderEngine.h"
#include "Object/ReObject/ReMaterial.h"
#include "Object/ReObject/RenderItem.h"
#include "Object/ReObject/ReMesh.h"
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
	auto transform = GetTransform();
	auto window_size = GetOwnerWindow()->GetTransform()->GetSize();
	if (transform->IsDirty() || 
		(window_size != m_PrevWindowSize))
	{
		m_PrevWindowSize = window_size;
		auto pos = transform->GetPosition();
		auto angle = transform->GetAngle();
		auto scale = transform->GetScale();
		pos = GetOwnerWindow()->ConvertToScreenPos(pos);


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
void JGUIShape::Resize(const JGUIResizeEvent& e)
{
	if (m_RenderItem == nullptr)	SetParent(GetParent());
	if (m_RenderItem)
	{
		m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(e.width, e.height));
	}

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

	FindPanel();
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
		RE::RenderEngine::DestroyRenderItem(GetOwnerWindow()->GetID(), m_RenderItem);
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
			if (cnt == 1 && m_OwnerPanel->GetParent() == nullptr)
			{
				m_Priority = GetOwnerWindow()->GetPriority();
			}
			else  m_Priority = GetOwnerWindow()->GetPriority() + cnt;
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
		CreateRI();
	}
}
void JGUIShape::CreateRI()
{
}
void JGUIRectangle::SetImage(const std::string& texture_image)
{
	if (m_RenderItem)
	{
		m_RenderItem->SetMaterial(RE_GUI_OneTextureDefault);
		m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
		m_RenderItem->GetMaterial()->SetTexture("Image", texture_image);
	}

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
	m_RenderItem->SetMaterial(RE_GUI_DefaultMaterial);
	m_RenderItem->GetMaterial()->SetValueAsFloat4("Color", m_Color);
	m_RenderItem->SetMesh(RE::ReGuiMesh::CreateFillRect(size.x, size.y));
	m_RenderItem->SetPriority(m_Priority);
	m_Instance = m_RenderItem->AddInstance();
	m_RenderItem->SetActive(IsActive());

}



