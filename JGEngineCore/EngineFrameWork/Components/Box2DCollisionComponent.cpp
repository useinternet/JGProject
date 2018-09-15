#include"Box2DCollisionComponent.h"
#include"StaticMesh2DComponent.h"
#include"../World/World.h"
#include"../../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../../EngineStatics/JGConstructHelper.h"
#include"../../PhysicsSystem/JGShape/JGRectangle.h"

#include"../../PhysicsSystem/PhysicsWorld.h"
#include"../../EngineStatics/JGLog.h"
using namespace std;

Box2DCollisionComponent::Box2DCollisionComponent()
{
	RegisterComponentID(typeid(this));
	m_BodyType = E2DBodyType::Static;
	m_2DBox = make_unique<JGRectangle>(EShapeType::Rectangle,
		GetComponentLocationAddress(), GetComponentAngleAddress());
}

Box2DCollisionComponent::~Box2DCollisionComponent()
{

}
void Box2DCollisionComponent::BeginComponent(World* world)
{
	CollisionComponent::BeginComponent(world);
	m_Body = GetWorld()->GetPhysicsWorld()->CreateBody(m_2DBox.get(), m_BodyType);
	m_Body->CreateCollisionChecker();
	if (m_bDebugRender)
	{
		InitDebugBoxResource();
	}
}
void Box2DCollisionComponent::Tick(const float DeltaTime)
{
	CollisionComponent::Tick(DeltaTime);
	if (m_bDebugRender && m_2DBox->GetIsTransRectangleSize())
	{
		float DebugBoxWidth = m_DebugRenderMesh->GetTextureWdith();
		float DebugBoxHeight = m_DebugRenderMesh->GetTextureHeight();
		float aspectWidth = m_2DBox->GetHalfWidth() / (DebugBoxWidth / 2);
		float aspectHeight = m_2DBox->GetHalfHeight() / (DebugBoxHeight / 2);
		m_DebugRenderMesh->SetComponentScale(aspectWidth, aspectHeight);
	}
}
void Box2DCollisionComponent::SetBox(const float h_Width, const float h_Height)
{
	m_2DBox->SetRectangle(h_Width, h_Height);
}
void Box2DCollisionComponent::SetCollisionType(E2DBodyType type)
{
	m_BodyType = type;
}

void Box2DCollisionComponent::DebugRenderingOn()
{
	m_bDebugRender = true;
}

void Box2DCollisionComponent::InitDebugBoxResource()
{
	m_DebugRenderMesh = make_unique<StaticMesh2DComponent>();
	m_DebugRenderMesh->RegisterName(GetComponentName() + TT("DebugRedner"));

	static JGConstructHelper::StaticMesh2D DebugMesh(
		GetDevice(), GetBufferManager(), m_DebugRenderMesh->GetComponentName(),
		EPivot::MiddleMiddle, TT("../ManagementFiles/Resource/DebugBox.png"));
	if (DebugMesh.Success)
	{
		m_DebugRenderMesh->SetConstructObject(DebugMesh.Object);
	}
	m_DebugRenderMesh->SpriteSwichOn(1.0f, 1.0f, 1.0f);
	AddChild(m_DebugRenderMesh.get());
}
