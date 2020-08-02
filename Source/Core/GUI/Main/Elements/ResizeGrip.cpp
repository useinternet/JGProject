#include "pch.h"
#include "ResizeGrip.h"
#include "Components/Transform.h"
#include "Components/DrawComponent.h"
#include "Windows/Window.h"
using namespace std;

namespace GUI
{
	void ResizeGrip::Awake()
	{
		m_ShapeComponent = CreateComponent<ShapeComponent>(TT("ShapeComponent"));
		m_ShapeComponent->BindConvertVertexCoordinate(
			[&](const JRect& localRect, std::vector<PaperVertex>& v, std::vector<uint32_t>& i)
		{
			if (v.empty() || i.empty())
			{
				v.resize(3);
				i.resize(3);
				v[0].texcoord = JVector2(0, 1);
				v[1].texcoord = JVector2(1, 0);
				v[2].texcoord = JVector2(1, 1);

				i[0] = 0; i[1] = 1; i[2] = 2;
			}

			v[0].position = JVector3(localRect.left, localRect.bottom, 0.0f);
			v[1].position = JVector3(localRect.right, localRect.top, 0.0f);
			v[2].position = JVector3(localRect.right, localRect.bottom, 0.0f);
		});
		m_ShapeComponent->BindInteractionActive(
			[&](const JRect& rect) -> bool
		{
			JVector2 p = GetOwnerWindow()->GetMousePos();
			return  rect.Contain(p);
		});
	}

	void ResizeGrip::Start()
	{

	}

	void ResizeGrip::Tick()
	{
		m_ResizeTick += GUIIF::GetTick();
		if (m_ResizeTick < m_ResizeTimePerTick) return;
		m_ResizeTick = 0.0f;
		if (m_IsResizing)
		{
			auto mousePos = GetOwnerWindow()->GetMousePos();
			auto delta = mousePos - m_MousePosWhilePressed;

			if (m_BindedTransform)
			{
				auto size = m_BindedTransform->GetSize();
				size.x = std::max<float>(m_MinSize.x, size.x + delta.x);
				size.y = std::max<float>(m_MinSize.y, size.y + delta.y);
				m_BindedTransform->SetSize(size);
			}
			m_MousePosWhilePressed = mousePos;
		}
		if (m_IsResizing || m_IsMouseIn)
		{
			m_ShapeComponent->SetColor(m_HightlightColor);
		}
		else
		{
			m_ShapeComponent->SetColor(m_NormalColor);
		}
	}

	void ResizeGrip::Destroy()
	{

	}

	void ResizeGrip::OnMouseButtonDown(KeyCode bt)
	{
		if (!m_IsResizing)
		{
			m_IsResizing = true;
			m_MousePosWhilePressed = GetOwnerWindow()->GetMousePos();
		}

	}

	void ResizeGrip::OnMouseButtonUp(KeyCode bt)
	{

		if (m_IsResizing) m_IsResizing = false;
	}

	void ResizeGrip::OnMouseEnter()
	{
		m_IsMouseIn = true;
	}

	void ResizeGrip::OnMouseExit()
	{
		m_IsMouseIn = false;
	}

	void ResizeGrip::BindTransform(Transform* transform)
	{
		m_BindedTransform = transform;
	}

	void ResizeGrip::SetHightlightColor(const JColor& color)
	{
		m_HightlightColor = color;
	}

	void ResizeGrip::SetNormalColor(const JColor& color)
	{
		m_NormalColor = color;
	}

}