#pragma once
#include"Motivated2DComponent.h"


// 각 몸체는 현제 자기가 가지고있는 콜리전 컴포넌트를 포인터 형태로 보관..
// 충돌 리스터에서  충돌중인 컬리전 컴포넌트를 보낸다..
// 이동 관련된거는 컬리전 컴포넌트 이후에 가상함수로 Box2D 함수로 대체..

//
class ENGINE_EXPORT CollisionComponent : public Motivated2DComponent
{
private:
	bool m_bBeginOverlap = false;
	bool m_bEndOverlap   = false;
	std::unique_ptr<JGVector2D> m_Velocity;
	std::vector<Object*> m_vOverlapObjects;
	Object* m_BeginOverlapObject = nullptr;
	Object* m_EndOverlapObject   = nullptr;
public:
	CollisionComponent();
	virtual ~CollisionComponent();
	virtual void Tick(const float DeltaTime) override;
	virtual void BeginOverlap(Object* OverlapObject);
	virtual void Overlapping(std::vector<Object*>& OverlapObjectArray);
	virtual void EndOverlap(Object* OverlapObject);
	void AddOverlapObject(Object* overlapObject);
	void DeleteOverlapObject(Object* overlapObject);
public:
	JGVector2D& GetVelocity();
	void SetVelocity(const JGVector2D& vel);


};