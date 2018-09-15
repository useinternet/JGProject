#include"PhysicsCalculationSystem.h"
#include"PhysicsWorld.h"
#include"JG2DBody.h"
#include"JGShape/JGShape.h"
#include"../EngineStatics/JMath/JGVector2D.h"
#include"JGCollisionCheckManager/JGCollisionCheck.h"
PhysicsCalculationSystem::PhysicsCalculationSystem(PhysicsWorld* pWorld)
{
	m_pPhysicsWorld = pWorld;

}
PhysicsCalculationSystem::~PhysicsCalculationSystem()
{


}

void PhysicsCalculationSystem::Calculation(const float DeltaTime)
{
	bool HorizonCollison = false;
	bool VerticalCollision = false;
	for (auto& iter : *m_pPhysicsWorld->GetBodyArray())
	{
		if (iter->GetType() == E2DBodyType::Dyanamic)
		{
			// 충돌이 일어날때
			if (iter->GetCollisionChecker() && !iter->GetCollisionChecker()->IsCurrentCollision())
			{
				iter->AddForce(0.0f, m_pPhysicsWorld->GetGravity() * DeltaTime);
			}
			else if (iter->GetCollisionChecker()->IsCurrentCollision())
			{
		
				// 밑에쪽이라면 위로 가는거 제한..
				if (iter->GetCollisionChecker()->IsDownWayCollision())
				{
					if (iter->GetShape()->GetLocation().Y() - iter->GetShape()->GetPrevLocation().Y() >= 0)
					{
						JGVector2D vec(iter->GetShape()->GetLocation().X(), iter->GetShape()->GetPrevLocation().Y());
						iter->GetShape()->SetLocation(vec);
						VerticalCollision = true;
					}
				}
				if (iter->GetCollisionChecker()->IsUpWayCollision())
				{
					if (iter->GetShape()->GetLocation().Y() - iter->GetShape()->GetPrevLocation().Y() <= 0)
					{
						JGVector2D vec(iter->GetShape()->GetLocation().X(), iter->GetShape()->GetPrevLocation().Y());
						iter->GetShape()->SetLocation(vec);
					}
				}
				if (iter->GetCollisionChecker()->IsRightWayCollision())
				{
					if (iter->GetShape()->GetLocation().X() - iter->GetShape()->GetPrevLocation().X() >= 0)
					{
						JGVector2D vec(iter->GetShape()->GetPrevLocation().X(), iter->GetShape()->GetLocation().Y());
						iter->GetShape()->SetLocation(vec);
						HorizonCollison = true;
					}
				
				}
				if (iter->GetCollisionChecker()->IsLeftWayCollision())
				{
					if (iter->GetShape()->GetLocation().X() - iter->GetShape()->GetPrevLocation().X() <= 0)
					{
						JGVector2D vec(iter->GetShape()->GetPrevLocation().X(), iter->GetShape()->GetLocation().Y());
						iter->GetShape()->SetLocation(vec);
						HorizonCollison = true;
					}
				}
				if (HorizonCollison)
				{
					iter->SetForce(0.0f, iter->GetForce().Y());
				}
				else if (VerticalCollision)
				{
					iter->SetForce(iter->GetForce().X(), 0.0f);
				}
			}
			iter->GetShape()->AddLocation(iter->GetForce());
		}
	}
}
