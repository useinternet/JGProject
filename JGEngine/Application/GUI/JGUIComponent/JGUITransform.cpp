#include "pch.h"
#include "JGUITransform.h"
#include "GUI/JGUIObject/JGUIElement.h"
#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIScreen.h"
using namespace Concurrency;

void JGUITransform::Awake()
{
	m_LocalPosition = { 0,0 };
	m_LocalAngle = 0.0f;
	m_Scale = { 1,1 };
	m_Size = { 100.0f,100.0f };

	for (int i = 0; i < DirtyCount; ++i)
	{
		m_IsDirty[i] = true;
	}
}

void JGUITransform::Destroy()
{
	ClearAttachedTransform();
	if (m_Attacher)
	{
		m_Attacher->DetachTransform(this);
		m_Attacher = nullptr;
	}
}

void JGUITransform::AttachTransform(JGUITransform* transform)
{
	//if (this == transform) return;
	if (transform == nullptr || transform->m_Attacher == this) return;

	if (transform->m_Attacher)
	{
		transform->m_Attacher->DetachTransform(transform);
	}
	transform->m_Attacher = this;
	transform->m_Size = m_Size;
	transform->m_Scale = m_Scale;
	transform->SendDirty();
	transform->SendAttachedTransform_Scale(m_Scale);
	transform->SendAttachedTransform_Size(m_Size);

	m_AttachedList.insert(transform);
}

void JGUITransform::DetachTransform(JGUITransform* transform)
{
	if (m_AttachedList.find(transform) == m_AttachedList.end()) return;
	if (transform)
	{
		transform->m_Attacher = nullptr;
	}
	m_AttachedList.erase(transform);

}
void JGUITransform::ClearAttachedTransform()
{
	for (auto& transform : m_AttachedList)
	{
		transform->m_Attacher = nullptr;
	}
	m_AttachedList.clear();
	return;
}
void JGUITransform::SetLocalPosition(const JVector2& pos)
{
	if (m_Attacher) return;
	m_LocalPosition = { pos.x, pos.y };
	SendDirty(PosDirty);
	SendPosToWin();
}
void JGUITransform::SetLocalPosition(float x, float y)
{
	if (m_Attacher) return;
	m_LocalPosition = { x, y };
	SendDirty(PosDirty);
	SendPosToWin();
}
void JGUITransform::OffsetLocalPosition(const JVector2& offset)
{
	if (m_Attacher) return;
	m_LocalPosition.x += offset.x;
	m_LocalPosition.y += offset.y;
	SendDirty(PosDirty);
	SendPosToWin();
}
void JGUITransform::OffsetLocalPosition(float x, float y)
{
	if (m_Attacher) return;
	m_LocalPosition.x += x;
	m_LocalPosition.y += y;
	SendDirty(PosDirty);
	SendPosToWin();
}


void JGUITransform::SetLocalAngle(float angle)
{
	if (m_Attacher) return;
	m_LocalAngle = angle;
	SendDirty(AngleDirty);
}
void JGUITransform::OffsetLocalAngle(float angle)
{
	if (m_Attacher) return;
	m_LocalAngle += angle;
	SendDirty(AngleDirty);
}


void JGUITransform::SetLocalScale(const JVector2& scale)
{
	if (m_Attacher) return;
	m_Scale = { scale.x, scale.y };
	SendAttachedTransform_Scale(scale);
}
void JGUITransform::SetLocalScale(float x, float y)
{
	if (m_Attacher) return;
	m_Scale = { x, y };
	SendAttachedTransform_Scale(m_Scale);
}
void JGUITransform::OffsetLocalScale(const JVector2& scale)
{
	if (m_Attacher) return;
	m_Scale.x += scale.x;
	m_Scale.y += scale.y;
	SendAttachedTransform_Scale(scale);
}
void JGUITransform::OffsetLocalScale(float x, float y)
{
	if (m_Attacher) return;
	m_Scale.x += x;
	m_Scale.y += y;
	SendAttachedTransform_Scale(m_Scale);
}
void JGUITransform::SetSize(const JVector2& size)
{
	if (m_Attacher) return;
	if (size == m_Size) return;
	m_Size = size;
	SendDirty();
	SendAttachedTransform_Size(m_Size);
	
	SendSizeToElement();
	SendSizeToWin();
}
void JGUITransform::SetSize(float x, float y)
{
	if (m_Attacher) return;
	if (JVector2( x,y ) == m_Size) return;
	m_Size.x = x;
	m_Size.y = y;
	SendDirty();
	SendAttachedTransform_Size(m_Size);
	SendSizeToElement();
	SendSizeToWin();
}
void JGUITransform::OffsetSize(const JVector2& offset)
{
	if (m_Attacher) return;
	m_Size.x += offset.x;
	m_Size.y += offset.y;
	SendDirty();
	SendAttachedTransform_Size(m_Size);
	SendSizeToElement();
	SendSizeToWin();
}
void JGUITransform::OffsetSize(float x, float y)
{
	if (m_Attacher) return;
	m_Size.x += x;
	m_Size.y += y;
	SendDirty();
	SendAttachedTransform_Size(m_Size);
	SendSizeToElement();
	SendSizeToWin();
}
void JGUITransform::SetPivot(const JVector2& pivot)
{
	m_Pivot = pivot;
	SendDirty(PivotDirty);
}
void JGUITransform::SetPivot(float x, float y)
{
	m_Pivot = { x,y };
	SendDirty(PivotDirty);
}
void JGUITransform::OffsetPivot(const JVector2& offset)
{
	m_Pivot += offset;
	SendDirty(PivotDirty);
}
void JGUITransform::OffsetPivot(float x, float y)
{
	m_Pivot += {x, y};
	SendDirty(PivotDirty);
}
bool JGUITransform::IsDirty() const
{

	for (auto& i : m_IsDirty)
	{
		if (i)
			return true;
	}
	return false;
}

void JGUITransform::SendDirty()
{
	SendDirty(UpdateDirty);
	if (m_ElementOwner)
	{
		for (uint32_t i = 0; i < m_ElementOwner->GetChildCount(); ++i)
		{
			m_ElementOwner->GetChild(i)->GetTransform()->SendDirty();
		}
	}
	if (m_WindowOwner)
	{
		for (auto& child : m_WindowOwner->GetChilds())
		{
			child->GetTransform()->SendDirty();
		}
		for (auto& element : m_WindowOwner->GetElements())
		{
			element->GetTransform()->SendDirty();
		}
	}
}
void JGUITransform::SendPosDirty()
{
	SendDirty(PosDirty);
}
void JGUITransform::SendAngleDirty()
{
	SendDirty(AngleDirty);

}
void JGUITransform::Flush()
{
	GetPosition();
	GetAngle();
	
	for (auto& i : m_IsDirty)
	{
		i = false;
	}

}

const JVector2& JGUITransform::GetLocalPosition() const
{
	return m_LocalPosition;
}
const JVector2& JGUITransform::GetPosition() const {
	if (m_Attacher) return m_Attacher->GetPosition();
	if (m_IsDirty[PosDirty])
	{
		m_Position = { 0,0 };

		if (m_WindowOwner)
		{
			auto binded_parent_win = m_WindowOwner->GetParent();
			if (binded_parent_win)
			{
				m_Position = binded_parent_win->GetTransform()->GetPosition();
			}
			m_Position.x += m_LocalPosition.x;
			m_Position.y += m_LocalPosition.y;
		}


		if (m_ElementOwner)
		{
			auto parent = m_ElementOwner->GetParent();
			if (parent)
			{
				m_Position = parent->GetTransform()->GetPosition();
			}
			m_Position.x += m_LocalPosition.x;
			m_Position.y += m_LocalPosition.y;
		}
		m_IsDirty[PosDirty] = false;
	}

	return m_Position;
}
float JGUITransform::GetLocalAngle() const
{
	return m_LocalAngle;
}
float JGUITransform::GetAngle() const
{
	if (m_Attacher) return m_Attacher->GetAngle();
	if (m_IsDirty[AngleDirty])
	{
		m_Angle = 0.0f;
		if (m_WindowOwner)
		{
			m_Angle = GetLocalAngle();
		}


		if (m_ElementOwner)
		{
			auto parent = m_ElementOwner->GetParent();
			if (parent)
			{
				m_Angle = parent->GetTransform()->GetAngle();
				m_Angle += GetLocalAngle();
			}
		}
		m_IsDirty[AngleDirty] = false;
	}

	return m_Angle;
}

const JVector2& JGUITransform::GetScale() const {
	return m_Scale;
}
const JVector2& JGUITransform::GetSize()  const {
	return m_Size;
}
const JVector2& JGUITransform::GetPivot() const
{
	if (m_Attacher) return m_Attacher->GetPivot();
	return m_Pivot;
}
const JVector2& JGUITransform::GetLocalPivot() const
{
	return m_LocalPivot;
}
JGUIRect JGUITransform::GetRect()  const {

	JGUIRect rect;
	float width = m_Size.x;
	float height = m_Size.y;

	auto pos = GetPosition();
	auto pivot = GetPivot();

	pos.x -= (pivot.x * width);
	pos.y -= (pivot.y * height);

	rect.left = pos.x;
	rect.top = pos.y;

	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	return rect;
}

JGUIRect JGUITransform::GetLocalRect() const
{
	JGUIRect rect;
	float width = m_Size.x;
	float height = m_Size.y;



	auto pivot = GetPivot();
	rect.left = m_LocalPosition.x;
	rect.top = m_LocalPosition.y;

	rect.left -= (pivot.x * width);
	rect.top -= (pivot.y * height);

	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	return rect;
}
void JGUITransform::SendDirty(int n)
{
	m_IsDirty[n] = true;
	m_IsDirty[UpdateDirty] = true;



	for (auto& attached : m_AttachedList)
	{
		attached->SendDirty(n);
	}

	if (m_WindowOwner)
	{
		for (auto& child : m_WindowOwner->GetChilds())
		{
			child->GetTransform()->SendDirty(n);
		}
	}
	if (m_ElementOwner)
	{
		for (uint32_t i = 0; i < m_ElementOwner->GetChildCount(); ++i)
		{
			m_ElementOwner->GetChild(i)->GetTransform()->SendDirty(n);
		}
	}
}
void JGUITransform::SendAttachedTransform_Size(const JVector2& size)
{
	for (auto& attached_com : m_AttachedList)
	{
		attached_com->m_Size = size;
		attached_com->SendDirty();
		attached_com->SendAttachedTransform_Size(size);
	}
}
void JGUITransform::SendAttachedTransform_Scale(const JVector2& scale)
{
	for (auto& attached_com : m_AttachedList)
	{
		attached_com->m_Scale = scale;
		attached_com->SendDirty();
		attached_com->SendAttachedTransform_Scale(scale);
	}
}
void JGUITransform::SendPosToWin()
{
	if (m_WindowOwner == nullptr) return;
	if (m_WindowOwner->GetParent() == nullptr && m_WindowOwner->GetScreen())
	{
		uint32_t x = std::max<uint32_t>(0, (uint32_t)m_LocalPosition.x);
		uint32_t y = std::max<uint32_t>(0, (uint32_t)m_LocalPosition.y);
		m_WindowOwner->GetScreen()->GetJWin()->SetPosition(x, y);
	}
}
void JGUITransform::SendSizeToWin()
{
	if (m_WindowOwner == nullptr) return;
	uint32_t x = std::max<uint32_t>(1, (uint32_t)m_Size.x);
	uint32_t y = std::max<uint32_t>(1, (uint32_t)m_Size.y);
	m_Size.x = x;
	m_Size.y = y;

	if (m_WindowOwner->GetParent() == nullptr && m_WindowOwner->GetScreen())
	{
		m_WindowOwner->GetScreen()->GetJWin()->SetClientSize(x, y);
		auto childCount = m_WindowOwner->GetChilds().size();
		for (size_t i = 0; i < childCount; ++i)
		{
			m_WindowOwner->FindChild((uint32_t)i)->GetTransform()->SendDirty();
		}
	}

	JGUIResizeEvent e;
	e.width = m_Size.x;
	e.height = m_Size.y;
	m_WindowOwner->JGUIResize(e);
	
}
void JGUITransform::SendSizeToElement()
{
	if (m_ElementOwner == nullptr) return;
	uint32_t x = std::max<uint32_t>(1, (uint32_t)m_Size.x);
	uint32_t y = std::max<uint32_t>(1, (uint32_t)m_Size.y);
	m_Size.x = x;
	m_Size.y = y;

	JGUIResizeEvent e;
	e.width = m_Size.x;
	e.height = m_Size.y;
	m_ElementOwner->JGUIResize(e);

}

