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
	ChildLock();
}

void JGUIRectTransform::Destroy()
{
	if (m_Attached)
	{
		m_Attached->m_Attacher = nullptr;
		m_Attached = nullptr;
	}
	if (m_Attacher)
	{
		m_Attacher->m_Attached = nullptr;
		m_Attacher = nullptr;
	}
}
void JGUIRectTransform::AttachTransform(JGUIRectTransform* transform)
{
	if (m_Attached) // 이미 연결되 트랜스폼이 있었다면 연결 해제
	{
		m_Attached->m_Attacher = nullptr;
	}
	m_Attached = transform;
	if (m_Attached)
	{
		m_Attached->m_Attacher = this;

		JGUIExtraEvent e;
		e.Bind(JGUI_EXTRAEVENT_REPEAT, this, [&](JGUIExtraEvent& e)
		{
			if (m_Attached)
			{
				m_Attached->m_LocalPosition = m_LocalPosition;
				m_Attached->m_LocalAngle = m_LocalAngle;
				m_Attached->m_LocalPivot = m_LocalPivot;
				m_Attached->m_Scale = m_Scale;
				m_Attached->m_Size = m_Size;
				m_Attached->m_Pivot = m_Pivot;
				m_Attached->m_Position = m_Position;
				m_Attached->m_Angle = m_Angle;
				for (int i = 0; i < DirtyCount; ++i)
				{
					m_Attached->m_IsDirty[i] = m_IsDirty[i];
				}
			}
			else
			{
				e.flag = JGUI_EXTRAEVENT_EXIT;
			}
		});
		JGUI::RegisterExtraEvent(e);

	}

}


void JGUIRectTransform::SetPosition(const JVector2& pos)
{
	if (m_Attacher) return;
	m_LocalPosition = { pos.x, pos.y };
	SendDirty(PosDirty);
}
void JGUIRectTransform::SetPosition(float x, float y)
{
	if (m_Attacher) return;
	m_LocalPosition = { x, y };
	SendDirty(PosDirty);
}
void JGUIRectTransform::OffsetPosition(const JVector2& offset)
{
	if (m_Attacher) return;
	m_LocalPosition.x += offset.x;
	m_LocalPosition.y += offset.y;
	SendDirty(PosDirty);
}
void JGUIRectTransform::OffsetPosition(float x, float y)
{
	if (m_Attacher) return;
	m_LocalPosition.x += x;
	m_LocalPosition.y += y;
	SendDirty(PosDirty);
}
void JGUIRectTransform::SetAngle(float angle)
{
	if (m_Attacher) return;
	m_LocalAngle = angle;
	SendDirty(AngleDirty);
}
void JGUIRectTransform::OffsetAngle(float angle)
{
	if (m_Attacher) return;
	m_LocalAngle += angle;
	SendDirty(AngleDirty);
}


void JGUIRectTransform::SetScale(const JVector2& scale)
{
	if (m_Attacher) return;
	m_Scale = { scale.x, scale.y};
}
void JGUIRectTransform::SetScale(float x, float y)
{
	if (m_Attacher) return;
	m_Scale = { x, y };
}
void JGUIRectTransform::OffsetScale(const JVector2& scale)
{
	if (m_Attacher) return;
	m_Scale.x += scale.x;
	m_Scale.y += scale.y;
}
void JGUIRectTransform::OffsetScale(float x, float y)
{
	if (m_Attacher) return;
	m_Scale.x += x;
	m_Scale.y += y;
}
void JGUIRectTransform::SetSize(const JVector2& size)
{
	if (m_Attacher) return;
	m_Size = size;
}
void JGUIRectTransform::SetSize(float x, float y)
{
	if (m_Attacher) return;
	m_Size.x = x;
	m_Size.y = y;
}
void JGUIRectTransform::OffsetSize(const JVector2& offset)
{
	if (m_Attacher) return;
	m_Size.x += offset.x;
	m_Size.y += offset.y;
}
void JGUIRectTransform::OffsetSize(float x, float y)
{
	if (m_Attacher) return;
	m_Size.x += x;
	m_Size.y += y;
}
void JGUIRectTransform::SetPivot(const JVector2& pivot)
{
	if (m_Attacher) return;
	m_LocalPivot = pivot;
	SendDirty(PivotDirty);
}
void JGUIRectTransform::SetPivot(float x, float y)
{
	if (m_Attacher) return;
	m_LocalPivot = { x,y };
	SendDirty(PivotDirty);
}
void JGUIRectTransform::OffsetPivot(const JVector2& pivot)
{
	if (m_Attacher) return;
	m_LocalPivot.x += pivot.x;
	m_LocalPivot.y += pivot.y;
	SendDirty(PivotDirty);
}
void JGUIRectTransform::OffsetPivot(float x, float y)
{
	if (m_Attacher) return;
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
	auto* parent = GetParent();
	if (parent)
	{
		parent = parent->GetParent();
	}
	// 부모가 컴퍼넌트
	if (parent)
	{
		parent->GetTransform()->Flush();
	}
	// 직속 윈도우 트랜스폼
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
	if (m_Attacher)
	{
		m_IsDirty[PosDirty] = false;
		return m_Position;
	}
	if (m_IsDirty[PosDirty])
	{
		auto* parent = GetParent();
		if (parent)
		{
			parent = parent->GetParent();
		}
		// 부모가 컴퍼넌트
		if (parent)
		{
			auto parent_transform = parent->GetTransform();
			m_Position = parent_transform->GetPosition();
			m_Position.x += m_LocalPosition.x;
			m_Position.y += m_LocalPosition.y;
		}
		// 직속 윈도우 트랜스폼
		else
		{
			m_Position.x = m_LocalPosition.x;
			m_Position.y = m_LocalPosition.y;
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
	if (m_Attacher)
	{
		m_IsDirty[AngleDirty] = false;
		return m_Angle;
	}
	if (m_IsDirty[AngleDirty])
	{
		auto* parent = GetParent();
		if (parent)
		{
			parent = parent->GetParent();
		}
		// 부모가 컴퍼넌트
		if (parent)
		{
			auto parent_transform = parent->GetTransform();
			m_Angle = parent_transform->GetAngle();
			m_Angle += GetLocalAngle();
		}
		// 직속 윈도우 트랜스폼
		else
		{
			m_Angle = GetLocalAngle();
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
	if (m_Attacher)
	{
		m_IsDirty[PivotDirty] = false;
		return m_Pivot;
	}
	if (m_IsDirty[PivotDirty])
	{
		auto* parent = GetParent();
		if (parent)
		{
			parent = parent->GetParent();
		}
		// 부모가 컴퍼넌트
		if (parent)
		{
			auto parent_transform = parent->GetTransform();
			m_Pivot = parent_transform->GetPivot();
			m_Pivot.x += m_LocalPivot.x;
			m_Pivot.y += m_LocalPivot.y;
		}
		// 직속 윈도우 트랜스폼
		else
		{
			m_Pivot.x = m_LocalPivot.x;
			m_Pivot.y = m_LocalPivot.y;
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
	m_IsDirty[UpdateDirty] = true;
	auto p = GetParent();
	if (p)
	{
		auto& child_array = p->GetChilds();
		for (auto& child : child_array)
		{
			if (child->GetTransform() == nullptr) continue;
			child->GetTransform()->SendDirty(n);
			//child->GetTransform()->m_IsDirty[n] = true;
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
			//com->GetTransform()->m_IsDirty[n] = true;
		}

	}
}


void JGUIWinRectTransform::SetPosition(const JVector2& pos)
{

	JGUIRectTransform::SetPosition(pos);
	SendPosToWin();
}

void JGUIWinRectTransform::SetPosition(float x, float y)
{
	JGUIRectTransform::SetPosition(x,y);
	SendPosToWin();
}

void JGUIWinRectTransform::OffsetPosition(const JVector2& offset)
{
	JGUIRectTransform::OffsetPosition(offset);
	SendPosToWin();
}

void JGUIWinRectTransform::OffsetPosition(float x, float y)
{
	JGUIRectTransform::OffsetPosition(x,y);
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
		uint32_t x = std::max<uint32_t>(0, (uint32_t)m_Size.x);
		uint32_t y = std::max<uint32_t>(0, (uint32_t)m_Size.y);


		GetOwnerWindow()->GetScreen()->GetJWin()->SetClientSize(x, y);
	}
}