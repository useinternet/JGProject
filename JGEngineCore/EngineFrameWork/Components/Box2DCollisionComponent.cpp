#include"Box2DCollisionComponent.h"
#include"StaticMesh2DComponent.h"
#include"../World/World.h"
#include"../../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../../EngineStatics/JGConstructHelper.h"
#include"../../PhysicsSystem/JGRectangle.h"

#include"../../PhysicsSystem/PhysicsWorld.h"
#include"../../EngineStatics/JGLog.h"
using namespace std;

Box2DCollisionComponent::Box2DCollisionComponent()
{
	RegisterComponentID(typeid(this));
	m_2DBox = make_unique<JGRectangle>(EShapeType::Rectangle);
}

Box2DCollisionComponent::~Box2DCollisionComponent()
{
}
void Box2DCollisionComponent::BeginComponent(World* world)
{
	CollisionComponent::BeginComponent(world);
	m_Body = GetWorld()->GetPhysicsWorld()->CreateBody(m_2DBox.get(), m_BodyType, SJG2DBodyDesc{ 1.0f,1.0f,0.0f });
}
void Box2DCollisionComponent::Tick(const float DeltaTime)
{
	CollisionComponent::Tick(DeltaTime);
	JGVector2D vec = GetComponentWorldLocation() - m_2DBox->GetLocation();
	AddComponentLocation(vec.X(), -vec.Y());
	m_2DBox->SetLocation(GetComponentWorldLocation());
	JGLog::Write(ELogLevel::Default, TT("�ӵ� : %d "), (int)(m_Body->GetVelocity()*100.0f));
}
void Box2DCollisionComponent::SettingCollision(const float h_Width, const float h_Height, const E2DBodyType bodyType)
{
	m_2DBox->SetLocation(GetComponentWorldLocation());
	m_2DBox->SetRectangle(h_Width, h_Height);
	m_2DBox->SetAngle(GetComponentWorldAngle());
	m_BodyType = bodyType;
}

void Box2DCollisionComponent::DebugRenderingOn()
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

	float DebugBoxWidth = m_DebugRenderMesh->GetTextureWdith();
	float DebugBoxHeight = m_DebugRenderMesh->GetTextureHeight();

	float aspectWidth = m_2DBox->GetHalfWidth() / (DebugBoxWidth / 2);
	float aspectHeight = m_2DBox->GetHalfHeight() / (DebugBoxHeight / 2);
	m_DebugRenderMesh->SpriteSwichOn(1.0f, 1.0f, 1.0f);
	m_DebugRenderMesh->SetComponentScale(aspectWidth, aspectHeight);
	AddChild(m_DebugRenderMesh.get());
}