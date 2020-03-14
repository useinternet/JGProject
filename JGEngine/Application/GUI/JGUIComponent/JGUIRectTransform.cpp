#include "pch.h"
#include "JGUIRectTransform.h"
#include "GUI/JGUIObject/JGUIWindow.h"
#include "GUI/JGUIScreen.h"
using namespace std;

JGUIRectTransform::JGUIRectTransform(const JVector2& pos, float angle, const JVector2& scale)
{
	m_LocalPosition = { pos.x, pos.y };
	m_LocalAngle = angle;
	m_Scale = { scale.x, scale.y };
	m_Size = { 100.0f,100.0f };

	for (int i = 0; i < DirtyCount; ++i)
	{
		m_IsDirty[i] = true;
	}
}

void JGUIRectTransform::Destroy()
{
	ClearAttachedTransform();
	if (m_Attacher)
	{
		m_Attacher->DetachTransform(this);
		m_Attacher = nullptr;
	}
}
void JGUIRectTransform::AttachTransform(JGUIRectTransform* transform)
{
	if (transform == nullptr|| transform->m_Attacher == this) return;
	
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
void JGUIRectTransform::DetachTransform(JGUIRectTransform* transform)
{
	if (m_AttachedList.find(transform) == m_AttachedList.end()) return;
	if (transform)
	{
		transform->m_Attacher = nullptr;
	}
	m_AttachedList.erase(transform);

}
void JGUIRectTransform::ClearAttachedTransform()
{
	for (auto& transform : m_AttachedList)
	{
		transform->m_Attacher = nullptr;
	}
	m_AttachedList.clear();
	return;
}
void JGUIRectTransform::SetLocalPosition(const JVector2& pos)
{
	if (m_Attacher) return;
	m_LocalPosition = { pos.x, pos.y };
	SendDirty(PosDirty);
}
void JGUIRectTransform::SetLocalPosition(float x, float y)
{
	if (m_Attacher) return;
	m_LocalPosition = { x, y };
	SendDirty(PosDirty);
}
void JGUIRectTransform::OffsetLocalPosition(const JVector2& offset)
{
	if (m_Attacher) return;
	m_LocalPosition.x += offset.x;
	m_LocalPosition.y += offset.y;
	SendDirty(PosDirty);
}
void JGUIRectTransform::OffsetLocalPosition(float x, float y)
{
	if (m_Attacher) return;
	m_LocalPosition.x += x;
	m_LocalPosition.y += y;
	SendDirty(PosDirty);
}


void JGUIRectTransform::SetLocalAngle(float angle)
{
	if (m_Attacher) return;
	m_LocalAngle = angle;
	SendDirty(AngleDirty);
}
void JGUIRectTransform::OffsetLocalAngle(float angle)
{
	if (m_Attacher) return;
	m_LocalAngle += angle;
	SendDirty(AngleDirty);
}


void JGUIRectTransform::SetLocalScale(const JVector2& scale)
{
	if (m_Attacher) return;
	m_Scale = { scale.x, scale.y};
	SendAttachedTransform_Scale(scale);
}
void JGUIRectTransform::SetLocalScale(float x, float y)
{
	if (m_Attacher) return;
	m_Scale = { x, y };
	SendAttachedTransform_Scale(m_Scale);
}
void JGUIRectTransform::OffsetLocalScale(const JVector2& scale)
{
	if (m_Attacher) return;
	m_Scale.x += scale.x;
	m_Scale.y += scale.y;
	SendAttachedTransform_Scale(scale);
}
void JGUIRectTransform::OffsetLocalScale(float x, float y)
{
	if (m_Attacher) return;
	m_Scale.x += x;
	m_Scale.y += y;
	SendAttachedTransform_Scale(m_Scale);
}
void JGUIRectTransform::SetSize(const JVector2& size)
{
	if (m_Attacher) return;
	m_Size = size;

	SendAttachedTransform_Size(m_Size);
}
void JGUIRectTransform::SetSize(float x, float y)
{
	if (m_Attacher) return;
	m_Size.x = x;
	m_Size.y = y;
	SendAttachedTransform_Size(m_Size);
}
void JGUIRectTransform::OffsetSize(const JVector2& offset)
{
	if (m_Attacher) return;
	m_Size.x += offset.x;
	m_Size.y += offset.y;
	SendAttachedTransform_Size(m_Size);
}
void JGUIRectTransform::OffsetSize(float x, float y)
{
	if (m_Attacher) return;
	m_Size.x += x;
	m_Size.y += y;
	SendAttachedTransform_Size(m_Size);
}
void JGUIRectTransform::SetPivot(const JVector2& pivot)
{
	m_Pivot = pivot;
	SendDirty(PivotDirty);
}
void JGUIRectTransform::SetPivot(float x, float y)
{
	m_Pivot = { x,y };
	SendDirty(PivotDirty);
}
void JGUIRectTransform::OffsetPivot(const JVector2& offset)
{
	m_Pivot += offset;
	SendDirty(PivotDirty);
}
void JGUIRectTransform::OffsetPivot(float x, float y)
{
	m_Pivot += {x, y};
	SendDirty(PivotDirty);
}
bool JGUIRectTransform::IsDirty() const
{
	
	for (auto& i : m_IsDirty)
	{
		if (i)
			return true;
	}
	return false;
}

void JGUIRectTransform::SendDirty()
{
	SendDirty(UpdateDirty);
}

void JGUIRectTransform::Flush()
{
	for (auto& i : m_IsDirty)
	{
		i = false;
	}

}

const JVector2& JGUIRectTransform::GetLocalPosition() const
{
	return m_LocalPosition;
}
const JVector2& JGUIRectTransform::GetPosition() const {
	if (m_Attacher) return m_Attacher->GetPosition();
	if (m_IsDirty[PosDirty])
	{
		m_Position = { 0,0 };

		if (m_BindedWindow)
		{
			m_Position.x = m_LocalPosition.x;
			m_Position.y = m_LocalPosition.y;
		}


		if (m_BindedComponent)
		{
			auto parent = m_BindedComponent->GetParent();
			if (parent)
			{
				m_Position = parent->GetTransform()->GetPosition();
				m_Position.x += m_LocalPosition.x;
				m_Position.y += m_LocalPosition.y;
			}
		}
		m_IsDirty[PosDirty] = false;
	}

	return m_Position;
}
float JGUIRectTransform::GetLocalAngle() const
{
	return m_LocalAngle;
}
float JGUIRectTransform::GetAngle() const
{
	if (m_Attacher) return m_Attacher->GetAngle();
	if (m_IsDirty[AngleDirty])
	{
		m_Angle = 0.0f;
		if (m_BindedWindow)
		{
			m_Angle = GetLocalAngle();
		}


		if (m_BindedComponent)
		{
			auto parent = m_BindedComponent->GetParent();
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

const JVector2& JGUIRectTransform::GetScale() const {
	return m_Scale;
}
const JVector2& JGUIRectTransform::GetSize()  const {
	return m_Size;
}
const JVector2& JGUIRectTransform::GetPivot() const
{
	if (m_Attacher) return m_Attacher->GetPivot();
	return m_Pivot;
}
const JVector2& JGUIRectTransform::GetLocalPivot() const
{
	return m_LocalPivot;
}
JGUIRect JGUIRectTransform::GetRect()  const {

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

JGUIRect JGUIRectTransform::GetLocalRect() const
{
	JGUIRect rect;
	float width = m_Size.x;
	float height = m_Size.y;



	auto pivot = GetPivot();
	rect.left = m_LocalPosition.x;
	rect.top  = m_LocalPosition.y;

	rect.left -= (pivot.x * width);
	rect.top  -= (pivot.y * height);

	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	return rect;
}
void JGUIRectTransform::SendDirty(int n)
{
	m_IsDirty[n] = true;
	m_IsDirty[UpdateDirty] = true;



	for (auto& attached : m_AttachedList)
	{
		attached->SendDirty(n);
	}


	if (m_BindedComponent)
	{
		for (auto& child : m_BindedComponent->GetChilds())
		{
			child->GetTransform()->SendDirty(n);
		}
	}
}
void JGUIRectTransform::SendAttachedTransform_Size(const JVector2& size)
{
	for (auto& attached_com : m_AttachedList)
	{
		attached_com->m_Size = size;
		attached_com->SendDirty();
		attached_com->SendAttachedTransform_Size(size);
	}
}
void JGUIRectTransform::SendAttachedTransform_Scale(const JVector2& scale)
{
	for (auto& attached_com : m_AttachedList)
	{
		attached_com->m_Scale = scale;
		attached_com->SendDirty();
		attached_com->SendAttachedTransform_Scale(scale);
	}
}
void JGUIRectTransform::BindComponent(JGUIComponent* com)
{
	m_BindedComponent = com;
	m_BindedWindow = nullptr;
}

void JGUIRectTransform::BindWindow(JGUIWindow* win)
{
	m_BindedWindow = win;
	m_BindedComponent = nullptr;
}


void JGUIWinRectTransform::SetLocalPosition(const JVector2& pos)
{
	JGUIRectTransform::SetLocalPosition(pos);
	SendPosToWin();
}

void JGUIWinRectTransform::SetLocalPosition(float x, float y)
{
	JGUIRectTransform::SetLocalPosition(x,y);
	SendPosToWin();
}

void JGUIWinRectTransform::OffsetLocalPosition(const JVector2& offset)
{
	JGUIRectTransform::OffsetLocalPosition(offset);
	SendPosToWin();
}

void JGUIWinRectTransform::OffsetLocalPosition(float x, float y)
{
	JGUIRectTransform::OffsetLocalPosition(x,y);
	SendPosToWin();
}
void JGUIWinRectTransform::SetSize(const JVector2& size)
{
	JGUIRectTransform::SetSize(size);
	SendSizeToWin();
}
void JGUIWinRectTransform::SetSize(float x, float y)
{
	JGUIRectTransform::SetSize(x, y);
	SendSizeToWin();
}
void JGUIWinRectTransform::OffsetSize(const JVector2& offset)
{
	JGUIRectTransform::OffsetSize(offset);
	SendSizeToWin();
}
void JGUIWinRectTransform::OffsetSize(float x, float y)
{
	JGUIRectTransform::OffsetSize(x, y);
	SendSizeToWin();
}
void JGUIWinRectTransform::SendPosToWin()
{
	if (GetOwnerWindow()->GetParent() == nullptr && GetOwnerWindow()->GetScreen())
	{
		uint32_t x = std::max<uint32_t>(0, (uint32_t)m_LocalPosition.x);
		uint32_t y = std::max<uint32_t>(0, (uint32_t)m_LocalPosition.y);
		GetOwnerWindow()->GetScreen()->GetJWin()->SetPosition(x, y);
	}
	
}
void JGUIWinRectTransform::SendSizeToWin()
{
	if (GetOwnerWindow()->GetParent() == nullptr && GetOwnerWindow()->GetScreen())
	{
		uint32_t x = std::max<uint32_t>(200, (uint32_t)m_Size.x);
		uint32_t y = std::max<uint32_t>(200, (uint32_t)m_Size.y);
		m_Size.x = x;
		m_Size.y = y;
		GetOwnerWindow()->GetScreen()->GetJWin()->SetClientSize(x, y);

		auto childCount = GetOwnerWindow()->GetChilds().size();
		for (size_t i = 0; i < childCount; ++i)
		{
			GetOwnerWindow()->FindChild((uint32_t)i)->GetTransform()->SendDirty();
		}
	}
	else 
	{
		JGUIResizeEvent e;
		e.width  = m_Size.x;
		e.height = m_Size.y;
		
		GetOwnerWindow()->JGUIResize(e);
	}
}