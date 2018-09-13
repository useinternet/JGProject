#include"JGCollisionCheckManager.h"
#include"../JG2DBody.h"
#include"../JGShape/JGShape.h"
#include"../JGShape/JGRectangle.h"
#include"../../EngineStatics/JMath/JGVector2D.h"
#include"../PhysicsWorld.h"


JGCollisionCheckManager::JGCollisionCheckManager(PhysicsWorld * world)
{
}

JGCollisionCheckManager::~JGCollisionCheckManager()
{
}

void JGCollisionCheckManager::Tick()
{
	for (auto& Collision1 : *(m_pPhysicsWorld->GetBodyArray()))
	{
		if (Collision1->GetType() != E2DBodyType::Dyanamic)
		{
			continue;
		}
		for (auto& Collision2 : *(m_pPhysicsWorld->GetBodyArray()))
		{
			CollisionCheck(Collision1->GetShape(), Collision2->GetShape());
		}
	}
}
bool JGCollisionCheckManager::CollisionCheck(JGShape* Shape1, JGShape* Shape2)
{
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
	if (Rectangle1->GetLeftTop().X() <= Rectangle2->GetRightBottom().X() &&
		Rectangle1->GetRightBottom().X() >= Rectangle2->GetLeftTop().X() &&
		Rectangle1->GetLeftTop().Y() >= Rectangle2->GetRightBottom().Y() &&
		Rectangle1->GetRightBottom().Y() >= Rectangle2->GetLeftTop().Y())
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


