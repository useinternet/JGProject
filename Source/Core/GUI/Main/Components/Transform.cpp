#include "pch.h"
#include "Transform.h"
#include "Elements/Element.h"
#include "Windows/Window.h"

namespace JGUI
{
	void Transform::SetLocalLocation(const JVector2& pos)
	{
		SendDirty();
		m_LocalLocation = pos;
		CalcAnchorDistance();
	}

	void Transform::SetLocation(const JVector2& pos)
	{
		OffsetLocalLocation(pos - GetLocation());
	}

	void Transform::OffsetLocalLocation(const JVector2& offset)
	{
		SetLocalLocation(m_LocalLocation + offset);
	}

	void Transform::OffsetLocation(const JVector2& offset)
	{
		SetLocation(GetLocation() + offset);
	}


	void Transform::SetSize(const JVector2& size)
	{
		SendDirty(false);
		m_Size = size;
		CalcAnchorDistance();
		for (auto& childs : m_Childs)
		{
			childs->AnchorProcess();
		}

	}

	void Transform::OffsetSize(const JVector2& offset)
	{
		SetSize(m_Size + offset);
	}

	void Transform::SetAnchor(const JRect& anchor)
	{
		SendDirty();
		m_Anchor = anchor;
		CalcAnchorDistance();
	}
	void Transform::SetAnchor(AnchorPreset   preset)
	{
		switch (preset)
		{
			case AnchorPreset::Left_Top:      SetAnchor(0.0f,0.0f,0.0f,0.0f); break;
			case AnchorPreset::Center_Top:	  SetAnchor(0.5f,0.5f,0.0f,0.0f); break;
			case AnchorPreset::Right_Top:	  SetAnchor(1.0f,1.0f,0.0f,0.0f); break;
			case AnchorPreset::Left_Center:	  SetAnchor(0.0f,0.0f,0.5f,0.5f); break;
			case AnchorPreset::Center_Center: SetAnchor(0.5f,0.5f,0.5f,0.5f); break;
			case AnchorPreset::Right_Center:  SetAnchor(1.0f,1.0f,0.5f,0.5f); break;
			case AnchorPreset::Left_Bottom:	  SetAnchor(0.0f,0.0f,1.0f,1.0f); break;
			case AnchorPreset::Center_Bottom: SetAnchor(0.5f,0.5f,1.0f,1.0f); break;
			case AnchorPreset::Right_Bottom:  SetAnchor(1.0f,1.0f,1.0f,1.0f); break;

			case AnchorPreset::Top_HStretch:	  SetAnchor(0.0f, 1.0f, 0.0f, 0.0f); break;
			case AnchorPreset::Center_HStretch:	  SetAnchor(0.0f, 1.0f, 0.5f, 0.5f); break;
			case AnchorPreset::Bottom_HStretch:	  SetAnchor(0.0f, 1.0f, 1.0f, 1.0f); break;
			case AnchorPreset::Left_VStretch:	  SetAnchor(0.0f, 0.0f, 0.0f, 1.0f); break;
			case AnchorPreset::Center_VStretch:	  SetAnchor(0.5f, 0.5f, 0.0f, 1.0f); break;
			case AnchorPreset::Right_VStretch:	  SetAnchor(1.0f, 1.0f, 0.0f, 1.0f); break;
			case AnchorPreset::Full_Stretch:	  SetAnchor(0.0f, 1.0f, 0.0f, 1.0f); break;
		}					   					
	}
	void Transform::SetPivot(const JVector2& pivot)
	{
		SendDirty(false);
		m_Pivot = pivot;
		CalcAnchorDistance();
	}
	 
	const JVector2& Transform::GetLocalLocation() const
	{
		return m_LocalLocation;
	}
	const JVector2& Transform::GetLocation() const
	{
		UpdateLocation();
		return m_Location;
	}

	
	const JVector2& Transform::GetSize() const
	{
		return m_Size;
	}
	const JRect& Transform::GetAnchor() const
	{
		return m_Anchor;
	}
	const JVector2& Transform::GetPivot() const
	{
		return m_Pivot;
	}


	JRect Transform::GetLocalRect() const
	{
		JRect result(0.0f,0.0f, m_Size.x, m_Size.y);
		JVector2 localLocation = GetLocalLocation();

		float pivot_x = m_Size.x * m_Pivot.x;
		float pivot_y = m_Size.y * m_Pivot.y;
		float add_x = localLocation.x - pivot_x;
		float add_y = localLocation.y - pivot_y;


		result.left   += add_x;
		result.right  += add_x;

		result.top    += add_y;
		result.bottom += add_y;

		return result;
	}

	JRect Transform::GetRect()
	{
		JRect result(0.0f, 0.0f, m_Size.x, m_Size.y);
		JVector2 location = GetLocation();

		float pivot_x = m_Size.x * m_Pivot.x;
		float pivot_y = m_Size.y * m_Pivot.y;
		float add_x = location.x - pivot_x;
		float add_y = location.y - pivot_y;


		result.left  += add_x;
		result.right += add_x;

		result.top    += add_y;
		result.bottom += add_y;


		return result;
	}
	void Transform::SetParent(Transform* transform)
	{
		if (m_Parent == transform) return;
		if (m_Parent)
		{
			m_Parent->m_Childs.erase(std::remove_if(m_Parent->m_Childs.begin(), m_Parent->m_Childs.end(), [&](Transform* t)
			{
				return t == this;
			}));
		}

		m_Parent = transform;
		SendDirty(true, true);
		UpdateLocation();
		CalcAnchorDistance();
		if (m_Parent)
		{
			m_Parent->m_Childs.push_back(this);
			for (auto& child : m_Parent->m_Childs)
			{
				child->AnchorProcess();
			}
		}
	}

	Transform* Transform::GetChild(const std::wstring& name) const
	{
		auto iter = find_if(m_Childs.begin(), m_Childs.end(), [&](Transform* t)
		{
			return t->GetOwner()->GetName() == name;
		});
		return *iter;
	}

	Transform* Transform::GetChild(uint32_t idx) const
	{
		return m_Childs[idx];
	}





	const JMatrix& Transform::GetWorldMatrix() const
	{
		UpdateWorldMatrix();
		return m_WorldMatrix;
	}

	void Transform::SendDirty(bool is_location_self, bool is_send_layerDirty)
	{
		m_IsWorldUpdateDirty = true;
		if (is_location_self)
		{
			m_IsLocationDirty = true;
		}
		if (is_send_layerDirty)
		{
			if (m_Parent) GetOwner()->m_LayerComponent = m_Parent->GetOwner()->m_LayerComponent;
			else GetOwner()->m_LayerComponent = nullptr;
		}
		for (auto& child : m_Childs)
		{
			child->SendDirty(true, is_send_layerDirty);
		}
	}

	void Transform::CalcAnchorDistance()
	{
		if (m_Parent == nullptr)
		{
			m_DistanceFromAnchor = JRect();
			return;
		}

		auto  p = GetParent();
		auto  p_size = p->GetSize();
		auto p_location = p->GetLocalLocation();
		auto p_pivot = p->GetPivot();
		float p_hw = p_size.x * 0.5f;
		float p_hh = p_size.y * 0.5f;

		JRect p_rect(0.0f, 0.0f, p_size.x, p_size.y);
		auto p_width  = p_rect.Width();
		auto p_height = p_rect.Height();



		JRect c_rect = GetLocalRect();

		c_rect.left   += (p_size.x * m_Anchor.left);
		c_rect.right  += (p_size.x * m_Anchor.left);
		c_rect.top += (p_size.y * m_Anchor.top);
		c_rect.bottom += (p_size.y * m_Anchor.top);

		m_DistanceFromAnchor.left   = c_rect.left   - (p_rect.left + p_size.x * m_Anchor.left);
		m_DistanceFromAnchor.right  = c_rect.right  - (p_rect.left + p_size.x * m_Anchor.right);
		m_DistanceFromAnchor.top    = c_rect.top    - (p_rect.top + p_size.y * m_Anchor.top);
		m_DistanceFromAnchor.bottom = c_rect.bottom - (p_rect.top + p_size.y * m_Anchor.bottom);
	}
	void Transform::AnchorProcess()
	{
		if (m_Parent == nullptr)
		{
			return;
		}

		auto  p = GetParent();
		auto  p_size = p->GetSize();
		auto  p_pivot = p->GetPivot();
		float p_hw = p_size.x * 0.5f;
		float p_hh = p_size.y * 0.5f;




		JRect anchorRect;
		anchorRect.left   = p_size.x * m_Anchor.left;
		anchorRect.right  = p_size.x * m_Anchor.right;
		anchorRect.top    = p_size.y * m_Anchor.top;
		anchorRect.bottom = p_size.y * m_Anchor.bottom;


		JRect rect;
		rect.left   = anchorRect.left + m_DistanceFromAnchor.left;
		rect.right  = anchorRect.right + m_DistanceFromAnchor.right;
		rect.top    = anchorRect.top + m_DistanceFromAnchor.top;
		rect.bottom = anchorRect.bottom + m_DistanceFromAnchor.bottom;




		auto c_pivot = GetPivot();
		rect.left   -= (p_size.x * m_Anchor.left);
		rect.right  -= (p_size.x * m_Anchor.left);
		//
		rect.top    -= (p_size.y * m_Anchor.top );
		rect.bottom -= (p_size.y * m_Anchor.top);


	
		SetLocalLocation(rect.left + rect.Width() * c_pivot.x, rect.top + rect.Height() * c_pivot.y);
		SetSize(rect.Width(), rect.Height());
	}
	void Transform::UpdateLocation() const
	{
		if (m_IsLocationDirty)
		{
			
			m_IsLocationDirty = false;
			m_Location   = GetLocalLocation();
			if (m_Parent)
			{
				auto p_size  = m_Parent->GetSize();
				auto p_pivot = m_Parent->GetPivot();
				m_Location.x += (p_size.x * m_Anchor.left - p_size.x * p_pivot.x);
				m_Location.y += (p_size.y * m_Anchor.top - p_size.y * p_pivot.y);


				auto p_Location = m_Parent->GetLocation();
				m_Location += p_Location;
			}

		}
	}
	void Transform::UpdateWorldMatrix() const
	{
		if (GetOwner() == nullptr)
		{
			GUILOG_ERROR("Owner가 존재하지 않습니다.");
		}
		if (GetOwner()->GetOwnerWindow() == nullptr) return;
		if (m_IsWorldUpdateDirty)
		{
			m_IsWorldUpdateDirty = false;
			JVector2 window_size = GetOwner()->GetOwnerWindow()->GetTransform()->GetSize();



			JVector2 pivot = GetPivot();
			JVector2 size  = GetSize();

			float hw = window_size.x * 0.5f;
			float hh = window_size.y * 0.5f;

			auto location = GetLocation();

			location.x -= hw;
			location.y = (hh - location.y);

			

			auto t = JMatrix::Translation(JVector3(location.x, location.y, 0.0f));
			auto r = JMatrix::Rotation(JVector3(0.0f, 0.0f, 0.0f));
			auto s = JMatrix::Scaling(JVector3(1.0f, 1.0f, 1.0f));



			m_WorldMatrix = JMatrix::Transpose(r * t * s);
		}
	}

}