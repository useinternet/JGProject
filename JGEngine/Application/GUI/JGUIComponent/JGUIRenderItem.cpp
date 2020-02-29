#include "pch.h"
#include "JGUIRenderItem.h"
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



void JGUIRenderItem::SetActive(bool is_active, bool is_hierarchy)
{
	JGUIComponent::SetActive(is_active, is_hierarchy);
	if (m_RenderItem)  m_RenderItem->SetActive(IsActive());
}


void JGUIRenderItem::Destroy()
{
	DestroyRI();
}
void JGUIRenderItem::RenderUpdate()
{
	if (m_RenderItem == nullptr) return;

	auto transform = GetTransform();

	JVector2 window_size = m_DrawingWindow->GetTransform()->GetSize();

	if (transform->IsDirty() ||
		(window_size != m_PrevWindowSize))
	{
		m_PrevWindowSize = window_size;
		auto pos = transform->GetPosition();
		pos += m_Offset;

		auto angle = transform->GetAngle();
		auto scale = transform->GetScale();
		pos = m_DrawingWindow->ConvertToScreenPos(pos);




		auto pivot = transform->GetPivot();
		pivot.x = 1.0f - (pivot.x + 0.5f);

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


		if (m_RenderItem && m_RenderItem->GetMaterial())
		{
		
			auto mat = GetRI()->GetMaterial();
			float width  = m_ClipRect.width();
			float height = m_ClipRect.height();
			JGUIRect rect = m_ClipRect;

			JVector2 lt = { rect.left, rect.top };
			JVector2 rb = { rect.right, rect.bottom };

			lt += transform->GetPosition();
			rb += transform->GetPosition();
			lt += m_Offset;
			rb += m_Offset;

			lt = m_DrawingWindow->ConvertToScreenPos(lt);
			rb = m_DrawingWindow->ConvertToScreenPos(rb);

			lt.x += (shape_size.x * pivot.x);
			lt.y += (shape_size.y * pivot.y);
			rb.x += (shape_size.x * pivot.x);
			rb.y += (shape_size.y * pivot.y);

			mat->SetValueAsFloat2("ClipX", { lt.x, rb.x });
			mat->SetValueAsFloat2("ClipY", { lt.y, rb.y });
		}


	}
}

void JGUIRenderItem::SetRIPriority(uint64_t priority)
{
	m_RIPriority = priority;
	if (m_RenderItem) m_RenderItem->SetPriority(priority);
}
void JGUIRenderItem::SetDrawingWindow(JGUIWindow* win)
{
	if (win == nullptr)
	{
		DestroyRI();
		m_DrawingWindow = nullptr;
	}
	else if (m_DrawingWindow != win)
	{
		m_DrawingWindow = win;
		if (m_RenderItem) DestroyRI();
		CreateRI();
	}
}
void     JGUIRenderItem::SetLayer(uint64_t layer)
{
	m_Layer = layer;
	if (m_RenderItem) m_RenderItem->SetLayer(layer);
}
uint64_t JGUIRenderItem::GetLayer() const
{
	return m_Layer;
}
void JGUIRenderItem::DestroyRI()
{
	if (m_RenderItem)
	{
		m_Instance = nullptr;
		RE::RenderEngine::DestroyRenderItem(m_DrawingWindow->GetID(), m_RenderItem);
		m_RenderItem    = nullptr;
	}
}

void JGUIRenderItem::CreateRI()
{
	if (m_DrawingWindow == nullptr) return;
	if (m_RenderItem)
	{
		DestroyRI();
	}

	m_RenderItem = RE::RenderEngine::CreateRenderItem(m_DrawingWindow->GetID(),
		RE::ERenderItemUsage::GUI, GetName() + "_RI");
	m_RenderItem->SetPriority(GetOwnerWindow()->GetPriority());
	m_RenderItem->SetLayer(m_Layer);
	m_Instance = m_RenderItem->AddInstance();
	m_RenderItem->SetActive(IsActive());
}
