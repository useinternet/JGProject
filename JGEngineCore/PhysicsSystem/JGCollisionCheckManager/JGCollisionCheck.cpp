#include"JGCollisionCheck.h"
#include"../../EngineStatics/JMath/JGVector2D.h"
#include"../PhysicsCalculationSystem.h"
#include"../JGShape/JGShape.h"
#include"../JG2DBody.h"
#include"../JGShape/JGRectangle.h"
#include"JGCollisionCheckManager.h"
#include"../../EngineStatics/JMath/JGMath.h"
JGCollisionCheck::JGCollisionCheck()
{

}

JGCollisionCheck::~JGCollisionCheck()
{


}

bool JGCollisionCheck::IsCurrentCollision()
{
	return m_bCurrentCollision;
}
bool JGCollisionCheck::IsUpWayCollision()
{
	if (m_pBody == nullptr)
	{
		return false;
	}
	for (auto& iter : m_lpOverlapBody)
	{
		if (iter.first.bUpCollision)
		{
			return true;
		}
	}
	return false;
}
bool JGCollisionCheck::IsDownWayCollision()
{
	if (m_pBody == nullptr)
	{
		return false;
	}
	for (auto& iter : m_lpOverlapBody)
	{
		if (iter.first.bDownCollision)
		{
			return true;
		}
	}
	return false;
}
bool JGCollisionCheck::IsRightWayCollision()
{
	if (m_pBody == nullptr)
	{
		return false;
	}
	for (auto& iter : m_lpOverlapBody)
	{
		if (iter.first.bRightCollision)
		{
			return true;
		}
	}
	return false;
}
bool JGCollisionCheck::IsLeftWayCollision()
{
	if (m_pBody == nullptr)
	{
		return false;
	}
	for (auto& iter : m_lpOverlapBody)
	{
		if (iter.first.bLeftCollision)
		{
			return true;
		}
	}
	return false;
}
void JGCollisionCheck::Tick()
{
	if (m_lpOverlapBody.size() == 0)
	{
		m_bCurrentCollision = false;
	}
	else
	{
		m_bCurrentCollision = true;
	}
}
void JGCollisionCheck::LinkBody(JG2DBody* Body)
{
	m_pBody = Body;
}
void JGCollisionCheck::ClearOverlapBodyList()
{
	m_lpOverlapBody.clear();
}
void JGCollisionCheck::AddOverlapList(JG2DBody* Body)
{
	for (auto& iter : m_lpOverlapBody)
	{
		if (iter.second == Body)
		{
			iter.first.bMomentCollision = false;
			return;
		}
	}
	SWhereCollision information;
	if (m_pBody)
	{
	   information = CheckCollisionWayByShapeType(Body);
	}
	//SWhereCollision information;
	//information.bMomentCollision = true;
	//information.bCollision = true;
	
	//if (m_pBody)
	//{



	//	JGVector2D vec = m_pBody->GetShape()->GetLocation() - m_pBody->GetShape()->GetPrevLocation();
	//	if (vec.X() < 0)
	//	{
	//		JGVector2D DirectionVector;
	//		DirectionVector.Set(vec.X(), 0.0f);
	//		if (DirectionVector.Dot(vec) > 0)
	//		{
	//			information.bLeftCollision = true;
	//		}
	//	}
	//	if (vec.X() > 0)
	//	{
	//		JGVector2D DirectionVector;
	//		DirectionVector.Set(vec.X(), 0.0f);
	//		if (DirectionVector.Dot(vec) > 0)
	//		{
	//			information.bRightCollision = true;
	//		}
	//	}
	//	if (vec.Y() < 0)
	//	{
	//		JGVector2D DirectionVector;
	//		DirectionVector.Set(0.0f, vec.Y());
	//		if (DirectionVector.Dot(vec) > 0)
	//		{
	//			information.bUpCollision = true;
	//		}
	//		
	//	}
	//	if (vec.Y() > 0)
	//	{
	//		JGVector2D DirectionVector;
	//		DirectionVector.Set(0.0f, vec.Y());
	//		if (DirectionVector.Dot(vec) > 0)
	//		{
	//			information.bDownCollision = true;
	//		}
	//	}
	//}
	m_lpOverlapBody.push_back(CollisionInformation(information,Body));
}

void JGCollisionCheck::DeleteOverlapList(JG2DBody* Body)
{
	m_lpOverlapBody.remove_if([Body](CollisionInformation& infor) -> bool
	{
		if (infor.second == Body)
		{
			return true;
		}
		return false;
	});
}

SWhereCollision JGCollisionCheck::CheckCollisionWayByShapeType(JG2DBody* Body)
{
	SWhereCollision information;
	information.bMomentCollision = true;
	information.bCollision = true;
	JGVector2D vec = m_pBody->GetShape()->GetLocation() - m_pBody->GetShape()->GetPrevLocation();

	switch (m_pBody->GetShape()->GetShapeType())
	{
	case EShapeType::Circle:
		switch (Body->GetShape()->GetShapeType())
		{
		case EShapeType::Circle:
			return information;
		case EShapeType::Rectangle:
			return information;
		}
		break;
	case EShapeType::Rectangle:
		switch (Body->GetShape()->GetShapeType())
		{
		case EShapeType::Circle:
			return information;
		case EShapeType::Rectangle:
			JGRectangle* Rect1 = dynamic_cast<JGRectangle*>(m_pBody->GetShape());
			JGRectangle* Rect2 = dynamic_cast<JGRectangle*>(Body->GetShape());
			if (!Rect1 || !Rect2)
			{
				break;
			}
			float LocationDeltaY = JGMath::Abs(m_pBody->GetShape()->GetPrevLocation().Y() - Body->GetShape()->GetPrevLocation().Y());
			float LocationDeltaX = JGMath::Abs(m_pBody->GetShape()->GetPrevLocation().X() - Body->GetShape()->GetPrevLocation().X());
			float ErrorDeltaY = (Rect1->GetHalfHeight() + Rect2->GetHalfHeight()) - LocationDeltaY;
			float ErrorDeltaX = (Rect1->GetHalfWidth() + Rect2->GetHalfWidth()) - LocationDeltaX;
			if (JGMath::Abs(ErrorDeltaY) > 1.0f && (Rect1->GetHalfHeight() + Rect2->GetHalfHeight()) > LocationDeltaY)
			{
				if (vec.X() < 0)
				{
					information.bLeftCollision = true;
				}
				if (vec.X() > 0)
				{
					information.bRightCollision = true;
				}
			}
			if (JGMath::Abs(ErrorDeltaX) > 1.0f && (Rect1->GetHalfWidth() + Rect2->GetHalfWidth()) > LocationDeltaX)
			{
				if (vec.Y() < 0)
				{
					information.bUpCollision = true;


				}
				if (vec.Y() > 0)
				{
					information.bDownCollision = true;
				}
			}

			return information;
		}
		break;
	}

	return information;
}
