#include"pch.h"
#include "Border.h"
#include "Components/DrawComponent.h"

using namespace std;

namespace GUI
{
	void Border::Awake()
	{
		m_ShapeCom = CreateComponent<ShapeComponent>(TT("ShapeComponent"));
		UpdateMesh();
		m_ShapeCom->SetInteractionActive(false);
		m_ShapeCom->BindConvertVertexCoordinate(
			[&](const JRect& localRect, vector<PaperVertex>& v, vector<uint32_t>& i)
		{
			v[0].position  = { localRect.left , localRect.top - m_Thickness , 0.0f };
			v[1].position  = { localRect.left , localRect.top               , 0.0f };
			v[2].position  = { localRect.right, localRect.top               , 0.0f };
			v[3].position  = { localRect.right, localRect.top - m_Thickness , 0.0f };

			v[4].position  = { localRect.left , localRect.bottom               , 0.0f };
			v[5].position  = { localRect.left , localRect.bottom + m_Thickness , 0.0f };
			v[6].position  = { localRect.right, localRect.bottom + m_Thickness , 0.0f };
			v[7].position  = { localRect.right, localRect.bottom               , 0.0f };

			v[8].position  = { localRect.left  + m_Thickness, localRect.bottom + m_Thickness , 0.0f };
			v[9].position  = { localRect.left  + m_Thickness, localRect.top    - m_Thickness , 0.0f };
			v[10].position = { localRect.right - m_Thickness, localRect.top    - m_Thickness , 0.0f };
			v[11].position = { localRect.right - m_Thickness, localRect.bottom + m_Thickness , 0.0f };
		});
	}
	void Border::Start()
	{
	}
	void Border::Tick()
	{
	}
	void Border::Destroy()
	{
	}
	void Border::SetDrawPriority(uint64_t priority)
	{
		m_ShapeCom->SetDrawPriority(priority);
	}
	void Border::SetAutoPriority(bool is_auto)
	{
		m_ShapeCom->SetAutoPriority(is_auto);
	}
	void Border::SetColor(const JColor& color)
	{
		m_ShapeCom->SetColor(color);
	}
	void Border::SetThickness(float thick)
	{
		if (m_Thickness != thick)
		{
			UpdateMesh();
			m_Thickness = thick;
		}
	}
	const JColor& Border::GetColor() const
	{
		return m_ShapeCom->GetColor();
	}
	void Border::UpdateMesh()
	{
		auto size = GetTransform()->GetSize();
		MeshGenerator::EmptyRectangle(size.x, size.y, 0.0f, m_Thickness, m_ShapeCom->V_Ref(), m_ShapeCom->I_Ref());
	}
}