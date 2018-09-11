#include"PyhicsBody2D.h"
#include"../EngineStatics/JMath/JGVector2D.h"
#include"../EngineStatics/JMath/JGAngle2D.h"
#include"../EngineStatics/JMath/JGVector2D.h"
#include"../EngineStatics/JGLog.h"
#include"../RenderSystem/JGMaterial/JG2DMesh.h"
using namespace std;
PyhicsBody2D::PyhicsBody2D()
{
	m_Composition = make_unique<SBodyComposition>();

}
PyhicsBody2D::~PyhicsBody2D()
{

}
void PyhicsBody2D::CreateBody(const EBodyType bodytype, const EPyhicsType pyhicstype, const SBodyComposition& Composition,
	const EPivot pivot, JGVector2D* pLocation, JGAngle2D* pAngle)
{
	m_BodyType = bodytype;
	m_PyhicsType = pyhicstype;
	m_pLinkingLocation = pLocation;
	m_pLinkingAngle = pAngle;
	*m_Composition = Composition;
	m_BodyState = EPyhicsBodyState::Active;
	switch (bodytype)
	{
	case EBodyType::Box:
		m_Figure = make_unique<Box>();
		break;
	case EBodyType::Circle:
		m_Figure = make_unique<Circle>();
		break;
	case EBodyType::Capsule:
		m_Figure = make_unique<Capsule>();
		break;
	}
}
void PyhicsBody2D::SetBox(const float width, const float height)
{
	if (m_pLinkingLocation == nullptr || m_pLinkingAngle == nullptr)
	{
		JGLog::Write(ELogLevel::Warning, TT("This Body is not Linking Location or Angle ."));
		return;
	}
	if (m_BodyType != EBodyType::Box)
	{
		JGLog::Write(ELogLevel::Warning, TT("This BodyType is not BoxType. "));
		return;
	}
	Box* box = dynamic_cast<Box*>(m_Figure.get());
	if (box == nullptr)
	{
		JGLog::Write(ELogLevel::Warning, TT("This BodyType is not BoxType. "));
		return;
	}
	switch(m_Pivot)
	{
	case EPivot::TopLeft:
		box->Set(m_pLinkingLocation->X(), m_pLinkingLocation->Y(),
			m_pLinkingLocation->X() + width, m_pLinkingLocation->Y() + height);
		break;
	case EPivot::TopMiddle:
		box->Set(m_pLinkingLocation->X() - (width / 2), m_pLinkingLocation->Y(),
			m_pLinkingLocation->X() + (width / 2), m_pLinkingLocation->Y() + height);
		break;
	case EPivot::TopRight:
		box->Set(m_pLinkingLocation->X() - width, m_pLinkingLocation->Y(),
			m_pLinkingLocation->X(), m_pLinkingLocation->Y() + height);
		break;
	case EPivot::MiddleLeft:
		box->Set(m_pLinkingLocation->X(), m_pLinkingLocation->Y() - (height / 2),
			m_pLinkingLocation->X() + width, m_pLinkingLocation->Y() + (height / 2));
		break;
	case EPivot::MiddleMiddle:
		box->Set(m_pLinkingLocation->X() - (width / 2), m_pLinkingLocation->Y() - (height / 2),
			m_pLinkingLocation->X() + (width / 2), m_pLinkingLocation->Y() + (height / 2));
		break;
	case EPivot::MiddleRight:
		box->Set(m_pLinkingLocation->X() - width, m_pLinkingLocation->Y() - (height / 2),
			m_pLinkingLocation->X(), m_pLinkingLocation->Y() + (height / 2));
		break;
	case EPivot::BottomLeft:
		box->Set(m_pLinkingLocation->X(), m_pLinkingLocation->Y() - height,
			m_pLinkingLocation->X() + width, m_pLinkingLocation->Y());
		break;
	case EPivot::BottomMiddle:
		box->Set(m_pLinkingLocation->X() - (width / 2), m_pLinkingLocation->Y() - height,
			m_pLinkingLocation->X() + (width / 2), m_pLinkingLocation->Y());
		break;
	case EPivot::BottomRight:
		box->Set(m_pLinkingLocation->X() - width, m_pLinkingLocation->Y() - height,
			m_pLinkingLocation->X(), m_pLinkingLocation->Y());
		break;
	}
}

void PyhicsBody2D::SetCircle()
{
	// 추후 구현..
}

void PyhicsBody2D::SetCapsule()
{
	// 추후 구현..
}

void PyhicsBody2D::Tick(const float DeltaTime, const float Gravity, std::vector<std::shared_ptr<PyhicsBody2D>>* bodyArray)
{
	for (auto& iter : *bodyArray)
	{
		if (iter.get() == this)
		{
			continue;
		}
		if (iter->GetBodyState() == EPyhicsBodyState::Active)
		{
			// 일단 박스만.. 충돌 체크
			Box* box1 = dynamic_cast<Box*>(m_Figure.get());
			Box* box2 = dynamic_cast<Box*>(iter->m_Figure.get());
			if (box1 != nullptr && box2 != nullptr)
			{
				bool result = BoxToBoxCollisionCheck(box1, box2);
			}
		}
	}
}

bool PyhicsBody2D::BoxToBoxCollisionCheck(Box * box1, Box * box2)
{
	if( ( (box1->Left <= box2->Left && box2->Left <= box1->Right) ||
		(  box1->Left <= box2->Right && box2->Right <= box1->Right))
		&&
		( (box1->Top <= box2->Top && box2->Top <= box1->Bottom)   ||
		(  box1->Top <= box2->Bottom && box2->Bottom <= box1->Bottom)) )
	{
		return true;
	}
	return false;
}

bool PyhicsBody2D::BoxToPointCollisionCheck(Box* box, JGVector2D& point)
{
	return false;
}

bool PyhicsBody2D::BoxToCircleCollisionCheck(Box* box, Circle* circle)
{
	return false;
}

bool PyhicsBody2D::CircleToPointCollisionCheck(Circle* circle, JGVector2D& point)
{
	return false;
}

EBodyType PyhicsBody2D::GetBodyType()
{
	return m_BodyType;
}

EPyhicsBodyState PyhicsBody2D::GetBodyState()
{
	return m_BodyState;
}


