#pragma once
#include"CollisionComponent.h"
#include"../../PhysicsSystem/JG2DBody.h"

class StaticMesh2DComponent;
class JGRectangle;
class PhysicsWorld;
enum class E2DBodyType;

/*
Class : Box2DCollisionComponent */
class ENGINE_EXPORT Box2DCollisionComponent : public CollisionComponent
{
private:
	std::unique_ptr<StaticMesh2DComponent> m_DebugRenderMesh;
	std::unique_ptr<JGRectangle> m_2DBox;
	E2DBodyType m_BodyType;
	JG2DBody* m_Body = nullptr;


	bool m_bDebugRender = false;
public:
	Box2DCollisionComponent();
	virtual ~Box2DCollisionComponent();
	virtual void BeginComponent(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	void SetBox(const float h_Width, const float h_Height);
	void SetCollisionType(E2DBodyType type);
	void DebugRenderingOn();
private:
	void InitDebugBoxResource();
};