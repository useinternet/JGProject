#pragma once
#include"../../EngineStatics/Engine.h"
#include"../DamageInformation/DamageInformationBase.h"
#include"ExistObject.h"

typedef unsigned int FallingCheck;
typedef unsigned int DirectionCheck;

class ENGINE_EXPORT Unit : public ExistObject
{
private:


private:
	class Box2DCollisionComponent* m_Collision;
	JGVector2D     m_Velocity;
	FallingCheck   m_FallingCheck;
	FallingCheck   m_PrevFallingCheck;
	DirectionCheck m_CurrentDirection;
	DirectionCheck m_PrevDirection;


	bool m_bWorking = false;
	bool m_bDamaged = false;
	std::queue<DamageInformationBase> m_qDamageInfor;
public:
	Unit();
	virtual ~Unit();
	virtual void BeginObject(World* world) override;
	virtual void Tick(const float DeltaTime) override;
	virtual void DefineDirection();
	virtual void DefineMove();
	Box2DCollisionComponent* GetCollision();
	bool IsWorking();

	FallingCheck GetFallingCheck();
	DirectionCheck GetDirectionCheck();
	void SetFallingCheck(FallingCheck check);
	void SetDirectionCheck(DirectionCheck check);

	void SetVelocity(const JGVector2D& vel);
	void SetVelocityX(const float velX);
	void SetVelocityY(const float velY);
	JGVector2D& GetVelocity();

	JGVector2D GetObjectLocation();

	virtual void BeginOverlap(Object* obj);
	virtual void EndOverlap(Object* obj);
	virtual void Overlapping(const std::vector<Object*>& ObjList);


	void SendDamage(const DamageInformationBase& dmg);
	void ReceiveDamage();

	virtual void ReceiveSingleDamageProcess(class SingleDamage* dmg);
	virtual void ReceiveSplashDamageProcess(class SplashDamage* dmg);
};