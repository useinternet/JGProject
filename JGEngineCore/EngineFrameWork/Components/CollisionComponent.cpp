#include"CollisionComponent.h"
#include"../Object/Object.h"
using namespace std;


CollisionComponent::CollisionComponent()
{
	RegisterComponentID(typeid(this));
	m_Velocity = make_unique<JGVector2D>();
}

CollisionComponent::~CollisionComponent()
{


}

void CollisionComponent::Tick(const float DeltaTime)
{
	Motivated2DComponent::Tick(DeltaTime);
	


	if (m_bEndOverlap)
	{
		EndOverlap(m_EndOverlapObject);
		for (auto iter = m_vOverlapObjects.begin();
			      iter < m_vOverlapObjects.end();)
		{
			if ((*iter) == m_EndOverlapObject)
			{
				iter = m_vOverlapObjects.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		m_EndOverlapObject = nullptr;
		m_bEndOverlap = false;
	}
	if (m_vOverlapObjects.size() != 0)
	{
		Overlapping(m_vOverlapObjects);
	}
	if (m_bBeginOverlap)
	{
		BeginOverlap(m_BeginOverlapObject);
		m_vOverlapObjects.push_back(m_BeginOverlapObject);
		m_BeginOverlapObject = nullptr;
		m_bBeginOverlap = false;
	}


}

void CollisionComponent::BeginOverlap(Object* OverlapObject) {}
void CollisionComponent::Overlapping(vector<Object*>& OverlapObjectArray) {}
void CollisionComponent::EndOverlap(Object* OverlapObject) {}


void CollisionComponent::AddOverlapObject(Object* overlapObject)
{
	m_bBeginOverlap = true;
	m_BeginOverlapObject = overlapObject;
}

void CollisionComponent::DeleteOverlapObject(Object* overlapObject)
{
	m_bEndOverlap = true;
	m_EndOverlapObject = overlapObject;
}
JGVector2D& CollisionComponent::GetVelocity()
{
	return *m_Velocity;
}
void CollisionComponent::SetVelocity(const JGVector2D& vel)
{
	m_Velocity->Set(vel);
}






