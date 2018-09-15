#pragma once
#include"../../EngineStatics/Engine.h"

class JGShape;
class JG2DBody;
class JGVector2D;
class JGCollisionCheckManager;
class PhysicsCalculationSystem;
typedef struct SWhereCollision
{
	bool bMomentCollision = false;
	bool bCollision = false;
	bool bLeftCollision = false;
	bool bRightCollision = false;
	bool bUpCollision = false;
	bool bDownCollision = false;
}SWhereCollision;
/*
class : JGCollisionCheck 
@m JGShape* m_pShape : 현재 연결된 도형
@m bool m_bCurrentCollision : 충동 중인가 여부.. */
class ENGINE_EXPORT JGCollisionCheck
{
	typedef std::pair<SWhereCollision, JG2DBody*> CollisionInformation;
private:
	friend JG2DBody;
	friend PhysicsCalculationSystem;
	friend JGCollisionCheckManager;
private:
	JG2DBody* m_pBody = nullptr;
	std::list<CollisionInformation> m_lpOverlapBody;
	bool m_bCurrentCollision  = false;
public:
	JGCollisionCheck();
	~JGCollisionCheck();
	bool IsCurrentCollision();
	bool IsUpWayCollision();
	bool IsDownWayCollision();
	bool IsRightWayCollision();
	bool IsLeftWayCollision();
	void Tick();
protected:
	void LinkBody(JG2DBody* Body);
	void ClearOverlapBodyList();
	void AddOverlapList(JG2DBody* Body);
	void DeleteOverlapList(JG2DBody* Body);
	SWhereCollision CheckCollisionWayByShapeType(JG2DBody* Body);

};