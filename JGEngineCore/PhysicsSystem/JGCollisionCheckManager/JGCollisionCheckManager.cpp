#include"JGCollisionCheckManager.h"
#include"JGCollisionCheck.h"
#include"../JG2DBody.h"
#include"../JGShape/JGShape.h"
#include"../JGShape/JGRectangle.h"
#include"../../EngineStatics/JMath/JGVector2D.h"
#include"JGCollisionCheck.h"
#include"../PhysicsWorld.h"
#include"../../EngineStatics/JGLog.h"

JGCollisionCheckManager::JGCollisionCheckManager(PhysicsWorld * world)
{
	m_pPhysicsWorld = world;
}

JGCollisionCheckManager::~JGCollisionCheckManager()
{
}

void JGCollisionCheckManager::Tick()
{
	for (auto& Collision1 : *(m_pPhysicsWorld->GetBodyArray()))
	{
		//  컬리전의 타입이 Dyanamic이 아니거나 충돌정보가 존재 하지 않을때 Continue
		if (Collision1->GetType() == E2DBodyType::Static || !Collision1->GetCollisionChecker())
		{
			continue;
		}
		for (auto& Collision2 : *(m_pPhysicsWorld->GetBodyArray()))
		{
			// 자기 자신이거나 충돌정보가 없으면 Continue
			if (Collision1.get() == Collision2.get() || !Collision2->GetCollisionChecker())
			{
				continue;
			}
			// 충돌 체크후 충돌 체커에 저장 정보..
			bool result = CollisionCheck(Collision1.get(), Collision2.get());
			if (result)
			{
				Collision1->GetCollisionChecker()->AddOverlapList(Collision2.get());
			}
			else
			{
				Collision1->GetCollisionChecker()->DeleteOverlapList(Collision2.get());
			}
		}
	}
}
bool JGCollisionCheckManager::CollisionCheck(JG2DBody* Body1, JG2DBody* Body2)
{
	JGShape* Shape1 = Body1->GetShape();
	JGShape* Shape2 = Body2->GetShape();

	bool result = false;

	switch (Shape1->GetShapeType())
	{
	case EShapeType::Circle:
		switch (Shape2->GetShapeType())
		{
		case EShapeType::Circle:
			break;
		case EShapeType::Rectangle:
			break;
		}
		break;
	case EShapeType::Rectangle:
		switch (Shape2->GetShapeType())
		{
		case EShapeType::Circle:
			break;
		case EShapeType::Rectangle:
			result = RectToRectCollisionCheck(Shape1, Shape2);
			break;
		}
		break;
	}
	return result;
}
bool JGCollisionCheckManager::RectToRectCollisionCheck(JGShape* Rect1, JGShape* Rect2)
{
	JGRectangle* Rectangle1 = dynamic_cast<JGRectangle*>(Rect1);
	JGRectangle* Rectangle2 = dynamic_cast<JGRectangle*>(Rect2);


	if (Rectangle1 == nullptr || Rectangle2 == nullptr)
	{
		return false;
	}
	// 충돌 체크
	if(Rectangle2->GetLeftTop().X() <= Rectangle1->GetRightBottom().X() &&
	   Rectangle1->GetLeftTop().X() <= Rectangle2->GetRightBottom().X() &&
	   Rectangle2->GetLeftTop().Y() <= Rectangle1->GetRightBottom().Y() &&
	   Rectangle1->GetLeftTop().Y() <= Rectangle2->GetRightBottom().Y())
	{
		return true;
	}
	return false;
}

bool JGCollisionCheckManager::RectToDotCollisionCheck()
{
	return false;
}

bool JGCollisionCheckManager::RectToLineCollisionCheck()
{
	return false;
}

bool JGCollisionCheckManager::RectToCircleCollisionCheck()
{
	return false;
}


