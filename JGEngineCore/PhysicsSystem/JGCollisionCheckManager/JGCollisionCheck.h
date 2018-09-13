#pragma once
#include"../../EngineStatics/Engine.h"

class JGShape;
class ENGINE_EXPORT JGCollisionCheck
{
private:
	JGShape* m_pShape = nullptr;
	JGShape* m_pOverlapShape = nullptr;
	bool m_bCurrentCollision = false;
public:
	JGCollisionCheck();
	~JGCollisionCheck();

	bool IsCollision(JGShape* pOutCurrentCollisionShape);

protected:

};