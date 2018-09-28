#pragma once
#include"CollisionComponent.h"

// 미리 정보들을 저장하고 BeginComponent함수에 물리적 바디 생성..

// 상속받은 트랜스폼 함수 모두 삭제
// Body 물리 함수를 그대로 이용
// 물리 현상을 키고끌수있게하고 킬경우 Box2D에서 사용하는 좌표를 그대로 이용하고 
// 끌겨우 SetTransForm을 이용하여 Box2D좌표를 JG좌표로 고정
class JGPolygonShape;
enum class EReSizePivot
{
	Up,
	Down,
	Middle
};
class ENGINE_EXPORT Box2DCollisionComponent : public CollisionComponent
{
private:
	std::unique_ptr<JGPolygonShape> m_Shape;
	float m_HalfWidth   = 50.0f;
	float m_HalfHeight  = 50.0f;
	bool  m_bInit = false;
public:
	Box2DCollisionComponent();
	virtual ~Box2DCollisionComponent();
	virtual void BeginComponent(World* world) override;
	void SetAsBox(const float hw, const float hh, EReSizePivot Pivot = EReSizePivot::Down);
};