#pragma once
#include"Object.h"


class ExistObject : public Object
{
private:
	// 실험을 해보자..
	class StaticMesh2DComponent* Breath;
	class StaticMesh2DComponent* Test;
public:
	ExistObject();
	virtual ~ExistObject();

	virtual void BeginObject() override;

	virtual void Tick(const float DeltaTime) override;
};