#pragma once
#include"../EngineStatics/Engine.h"
#include"PhysicsBodyType.h"
// F = ma;
// 중력 -9.8f;
// 마찰력 = 수직항력 * 마찰 계수
// 가속도
// 반발력 = 탄성계수 * 떨어진 높이

// 
// 피봇에따라 b2Vec2 -> JGVector2D;
// 애니메이션, 스태틱

class JGVector2D;
class JGAngle2D;
enum class EPivot;
enum class EPyhicsType
{
	Dynamic,
	Static,
	Kinematic
};
enum class EBodyType
{
	Circle,
	Box,
	Capsule
};
enum class EPyhicsBodyState
{
	Active,
	Behind,
	Destroy
};
typedef struct SBodyComposition
{
	float Mass;
	float Friction;
	float Repulsive;
}SBodyComposition;
class ENGINE_EXPORT PyhicsBody2D
{
private:
	EBodyType m_BodyType;
	EPyhicsType m_PyhicsType;
	JGVector2D* m_pLinkingLocation;
	JGAngle2D*  m_pLinkingAngle;
	EPivot m_Pivot;
	EPyhicsBodyState m_BodyState;
	std::unique_ptr<Figure> m_Figure;
	std::unique_ptr<SBodyComposition> m_Composition;
public:
	PyhicsBody2D();
	~PyhicsBody2D();
	void CreateBody(const EBodyType bodytype,const EPyhicsType pyhicstype,const SBodyComposition& Composition,
		const EPivot pivot,JGVector2D* pLocation, JGAngle2D* pAngle);
	/*
	Exp : 박스 셋팅. */
	void SetBox(const float width,const float height);
	/*
	Exp : 미구현 */
	void SetCircle();
	/*
	Exp : 미구현 */
	void SetCapsule();

	void Tick(const float DeltaTime,const float Gravity, std::vector<std::shared_ptr<PyhicsBody2D>>* bodyArray);
private:

	// 일다 구현부터 해보자
	bool BoxToBoxCollisionCheck(Box* box1,Box* box2);
	bool BoxToPointCollisionCheck(Box* box, JGVector2D& point);
	bool BoxToCircleCollisionCheck(Box* box, Circle* circle);
	bool CircleToPointCollisionCheck(Circle* circle,JGVector2D& point);
	/*
	Exp : 현제 물리 몸체 타입을 가져온다. */
	EBodyType GetBodyType();
	/*
	Exp : 현제 물리 몸체 상황을 가져온다. */
	EPyhicsBodyState GetBodyState();
};


