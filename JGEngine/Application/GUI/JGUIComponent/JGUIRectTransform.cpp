#include "pch.h"
#include "JGUIRectTransform.h"
#include "GUI/JGUIObject/JGUIWindow.h"

using namespace std;

JGUIRectTransform::JGUIRectTransform(const JVector2& pos, float angle, const JVector2& scale)
{
	m_LocalPosition = { pos.x, pos.y };
	m_LocalAngle = angle;
	m_Scale = { scale.x, scale.y };
	m_Size = { 100.0f,100.0f };
}

void JGUIRectTransform::SetPosition(const JVector2& pos)
{
	m_LocalPosition = { pos.x, pos.y };
	SendDirty(PosDirty);
}
void JGUIRectTransform::SetPosition(float x, float y)
{
	m_LocalPosition = { x, y };
	SendDirty(PosDirty);
}
void JGUIRectTransform::OffsetPosition(const JVector2& offset)
{
	m_LocalPosition.x += offset.x;
	m_LocalPosition.y += offset.y;
	SendDirty(PosDirty);
}
void JGUIRectTransform::OffsetPosition(float x, float y)
{
	m_LocalPosition.x += x;
	m_LocalPosition.y += y;
	SendDirty(PosDirty);
}
void JGUIRectTransform::SetAngle(float angle)
{
	m_LocalAngle = angle;
	SendDirty(AngleDirty);
}
void JGUIRectTransform::OffsetAngle(float angle)
{
	m_LocalAngle += angle;
	SendDirty(AngleDirty);
}


void JGUIRectTransform::SetScale(const JVector2& scale)
{
	m_Scale = { scale.x, scale.y};
}
void JGUIRectTransform::SetScale(float x, float y)
{
	m_Scale = { x, y };
}
void JGUIRectTransform::OffsetScale(const JVector2& scale)
{
	m_Scale.x += scale.x;
	m_Scale.y += scale.y;
}
void JGUIRectTransform::OffsetScale(float x, float y)
{
	m_Scale.x += x;
	m_Scale.y += y;
}
void JGUIRectTransform::SetSize(const JVector2& size)
{
	m_Size = size;
}
void JGUIRectTransform::SetSize(float x, float y)
{
	m_Size.x = x;
	m_Size.y = y;
}
void JGUIRectTransform::OffsetSize(const JVector2& offset)
{
	m_Size.x += offset.x;
	m_Size.y += offset.y;
}
void JGUIRectTransform::OffsetSize(float x, float y)
{
	m_Size.x += x;
	m_Size.y += y;
}
void JGUIRectTransform::SetPivot(const JVector2& pivot)
{
	m_LocalPivot = pivot;
	SendDirty(PivotDirty);
}
void JGUIRectTransform::SetPivot(float x, float y)
{
	m_LocalPivot = { x,y };
	SendDirty(PivotDirty);
}
void JGUIRectTransform::OffsetPivot(const JVector2& pivot)
{
	m_LocalPivot.x += pivot.x;
	m_LocalPivot.y += pivot.y;
	SendDirty(PivotDirty);
}
void JGUIRectTransform::OffsetPivot(float x, float y)
{
	m_LocalPivot.x += x;
	m_LocalPivot.y += y;
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

void JGUIRectTransform::Flush()
{
	for (auto& i : m_IsDirty)
	{
		i = false;
	}
	auto* parent = GetParent();
	if (parent)
	{
		parent = parent->GetParent();
	}
	// �θ� ���۳�Ʈ
	if (parent)
	{
		parent->GetTransform()->Flush();
	}
	// ���� ������ Ʈ������
	else
	{
		auto window = GetOwnerWindow();
		if (window->GetParent())
		{
			auto window_parent_transform = window->GetParent()->GetTransform();
			window_parent_transform->Flush();
		}
	}
}

const JVector2& JGUIRectTransform::GetLocalPosition() const
{
	return m_LocalPosition;
}
const JVector2& JGUIRectTransform::GetPosition() const {
	if (m_IsDirty[PosDirty])
	{
		auto* parent = GetParent();
		if (parent)
		{
			parent = parent->GetParent();
		}
		// �θ� ���۳�Ʈ
		if (parent)
		{
			auto parent_transform = parent->GetTransform();
			m_Position = parent_transform->GetPosition();
			m_Position.x += m_LocalPosition.x;
			m_Position.y += m_LocalPosition.y;
		}
		// ���� ������ Ʈ������
		else
		{
			auto window = GetOwnerWindow();
			if (window->GetParent())
			{
				auto window_parent_transform = window->GetParent()->GetTransform();
				m_Position = window_parent_transform->GetPosition();
				m_Position.x += m_LocalPosition.x;
				m_Position.y += m_LocalPosition.y;
			}
			else
			{
				m_Position = { m_LocalPosition.x, m_LocalPosition.y };
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
	if (m_IsDirty[AngleDirty])
	{
		auto* parent = GetParent();
		if (parent)
		{
			parent = parent->GetParent();
		}
		// �θ� ���۳�Ʈ
		if (parent)
		{
			auto parent_transform = parent->GetTransform();
			m_Angle = parent_transform->GetAngle();
			m_Angle += GetLocalAngle();
		}
		// ���� ������ Ʈ������
		else
		{
			auto window = GetOwnerWindow();
			if (window->GetParent())
			{
				auto window_parent_transform = window->GetParent()->GetTransform();
				m_Angle = window_parent_transform->GetAngle();
				m_Angle += GetLocalAngle();
			}
			else
			{
				m_Angle = GetLocalAngle();
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
	if (m_IsDirty[PivotDirty])
	{
		auto* parent = GetParent();
		if (parent)
		{
			parent = parent->GetParent();
		}
		// �θ� ���۳�Ʈ
		if (parent)
		{
			auto parent_transform = parent->GetTransform();
			m_Pivot = parent_transform->GetPivot();
			m_Pivot.x += m_LocalPivot.x;
			m_Pivot.y += m_LocalPivot.y;
		}
		// ���� ������ Ʈ������
		else
		{
			auto window = GetOwnerWindow();
			if (window->GetParent())
			{
				auto window_parent_transform = window->GetParent()->GetTransform();
				m_Pivot = window_parent_transform->GetPivot();
				m_Pivot.x += m_LocalPivot.x;
				m_Pivot.y += m_LocalPivot.y;
			}
			else
			{
				m_Pivot.x += m_LocalPivot.x;
				m_Pivot.y += m_LocalPivot.y;
			}
		}
		m_IsDirty[PivotDirty] = false;
	}

	return m_Pivot;
}
const JVector2& JGUIRectTransform::GetLocalPivot() const
{
	return m_LocalPivot;
}
JGUIRect JGUIRectTransform::GetRect()  const {

	JGUIRect rect;
	rect.width = m_Size.x;
	rect.height = m_Size.y;

	auto pos = GetPosition();
	auto pivot = GetPivot();

	pos.x -= (pivot.x * rect.width);
	pos.y -= (pivot.y * rect.height);

	rect.left = pos.x;
	rect.top = pos.y;

	rect.right = rect.left + rect.width;
	rect.bottom = rect.top + rect.height;

	return rect;
}

JGUIRect JGUIRectTransform::GetLocalRect() const
{
	JGUIRect rect;
	rect.width = m_Size.x;
	rect.height = m_Size.y;
	auto pivot = GetPivot();
	rect.left = m_LocalPosition.x;
	rect.top = m_LocalPosition.y;

	rect.left -= (pivot.x * rect.width);
	rect.top -= (pivot.y * rect.height);

	rect.right = rect.left + rect.width;
	rect.bottom = rect.top + rect.height;

	return rect;
}
void JGUIRectTransform::SendDirty(int n)
{
	m_IsDirty[n] = true;
	auto p = GetParent();
	if (p)
	{
		auto& child_array = p->GetChilds();
		for (auto& child : child_array)
		{
			if (child->GetTransform() == nullptr) continue;
			child->GetTransform()->SendDirty(n);
			child->GetTransform()->m_IsDirty[n] = true;
		}
	}
	else
	{
		auto w = GetOwnerWindow();
		auto& com_array = w->GetWindowComponents();
		for (auto& com : com_array)
		{
			if (com->GetTransform() == nullptr) continue;
			com->GetTransform()->SendDirty(n);
			com->GetTransform()->m_IsDirty[n] = true;
		}

	}
}