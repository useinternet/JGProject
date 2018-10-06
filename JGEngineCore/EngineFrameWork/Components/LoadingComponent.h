#pragma once
#include"Component.h"


class JTimerHandle;
class ENGINE_EXPORT LoadingComponent : public Component
{
private:
	bool  m_LoadingComplete = false;
	float m_DefaultGapTime  = 0.5f;
	float m_CurrentProgress = 0.0f;
	class JTimerHandle* LoadingTimerEvent = nullptr;
public:
	LoadingComponent();
	virtual ~LoadingComponent();

	void Tick(const float DeltaTime) override;
	void OpenWorld(const std::wstring& OpenWorldName);
	float GetCurrentProgress();
private:
	void LoadingComplete();
	void LoadProgress();
};
