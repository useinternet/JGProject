#include"CollisionComponent.h"
#include"../Object/Object.h"
#include"../World/World.h"
#include"../../PhysicsSystem/JGBox2D/JGDynamics/JGPhysicsWorld.h"
#include"../../EngineStatics/JGLog.h"
using namespace std;


CollisionComponent::CollisionComponent()
{
	RegisterComponentID(typeid(this));
	m_Velocity = make_unique<JGVector2D>();
	SetBeginOverlapEvent([](Object* object) {});
	SetOverlappingEvent([](const vector<Object*>&) {});
	SetEndOverlapEvent([](Object* object) {});
}
CollisionComponent::~CollisionComponent()
{


}
void CollisionComponent::BeginComponent(World* world)
{
	Motivated2DComponent::BeginComponent(world);
}

void CollisionComponent::Tick(const float DeltaTime)
{
	Motivated2DComponent::Tick(DeltaTime);
	if (m_bDestory)
	{
		return;
	}
	if (m_bEndOverlap)
	{
		m_EndOverlapEvent(m_EndOverlapObject);
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
		m_OverlappingEvent(m_vOverlapObjects);
	}
	if (m_bBeginOverlap)
	{
		m_BeginOverlapEvent(m_BeginOverlapObject);
	
		m_vOverlapObjects.push_back(m_BeginOverlapObject);
		m_BeginOverlapObject = nullptr;
		m_bBeginOverlap = false;
	}
}

void CollisionComponent::SetBeginOverlapEvent(const function<void(Object*)>& Event)
{
	m_BeginOverlapEvent = Event;
}

void CollisionComponent::SetEndOverlapEvent(const function<void(Object*)>& Event)
{
	m_EndOverlapEvent = Event;
}

void CollisionComponent::SetOverlappingEvent(const function<void(const std::vector<Object*>&)>& Event)
{
	m_OverlappingEvent = Event;
}
void CollisionComponent::AddOverlapObject(Object* overlapObject)
{
	for(auto& iter : m_vOverlapObjects)
	{
		if (iter == overlapObject)
		{
			return;
		}
	}
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

E2DBodyType CollisionComponent::GetBodyType()
{
	return m_BodyType;
}
JG2DBody* CollisionComponent::GetBody()
{
	return m_Body;
}
float CollisionComponent::GetDensity()
{
	return m_Density;
}
float CollisionComponent::GetFriction()
{
	return m_Friction;
}
float CollisionComponent::GetRestitution()
{
	return m_Restitution;
}
JG2DFilter& CollisionComponent::GetFilter()
{
	return m_BodyFilter;
}
void CollisionComponent::SetBodyType(E2DBodyType type)
{
	m_BodyType = type;
}
void CollisionComponent::SetBody(JG2DBody* body)
{
	m_Body = body;
}
void CollisionComponent::SetDensity(const float Density)
{
	m_Density = Density;
}
void CollisionComponent::SetFriction(const float Friction)
{
	m_Friction = Friction;
}
void CollisionComponent::SetRestitution(const float Restitution)
{
	m_Restitution = Restitution;
}
void CollisionComponent::SetFilter(JG2DFilter filter)
{
	m_BodyFilter = filter;
}

void CollisionComponent::SetCategoryFilter(const CollisionFilter filter)
{
	m_BodyFilter.Get().categoryBits = filter;
}

void CollisionComponent::SetMaskFilter(const CollisionFilter filter)
{
	m_BodyFilter.Get().maskBits = filter;
}

void CollisionComponent::AddMaskFilter(const CollisionFilter filter)
{
	m_BodyFilter.Get().maskBits |= filter;
}

void CollisionComponent::FixAngle()
{
	m_Body->origin()->SetFixedRotation(true);
}

void CollisionComponent::UnFixAngle()
{
	m_Body->origin()->SetFixedRotation(false);
}

void CollisionComponent::DestroyCollison()
{
	if (m_Body == nullptr)
	{
		return;
	}
	GetWorld()->GetPyWorld()->DestoryBody(m_Body);
	m_Body = nullptr;
	m_bDestory = true;
}



