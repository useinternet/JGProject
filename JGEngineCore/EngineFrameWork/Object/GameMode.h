#pragma once
#include"Object.h"


class ENGINE_EXPORT GameMode : public Object
{
private:
	bool m_bGameEnd = false;
public:
	GameMode();
	virtual ~GameMode();
	virtual void Tick(const float DeltaTime) override;
	virtual bool ConditionGameEnd();
	virtual void GameEndEvent(const float DeltaTime);
	bool IsGameEnd();
};