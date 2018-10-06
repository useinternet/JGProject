#include"LoadingComponent.h"
#include"../World/World.h"
#include"../World/WorldManager.h"
#include"../../EngineStatics/JThreadManager.h"
#include"../../EngineStatics/JTimerManager.h"
#include"../../EngineStatics/JGLog.h"
using namespace std;
LoadingComponent::LoadingComponent()
{
	RegisterComponentID(typeid(this));
}

LoadingComponent::~LoadingComponent()
{
}
void LoadingComponent::Tick(const float DeltaTime)
{
	Component::Tick(DeltaTime);
	if (m_LoadingComplete && m_CurrentProgress >= 0.9f)
	{
		m_CurrentProgress = 1.0f;
	}
}

void LoadingComponent::OpenWorld(const wstring& OpenWorldName)
{
	JTimerEventManager::CreateTimerEvent(&LoadingTimerEvent, bind(&LoadingComponent::LoadProgress, this),
		EHandleType::ERepeat, 0.0f, m_DefaultGapTime, -1);
	JThreadManager::AddTask([this,OpenWorldName]()
	{
		this->GetWorld()->GetManager()->OpenWorld(OpenWorldName);
		this->LoadingComplete();
	});
}

float LoadingComponent::GetCurrentProgress()
{
	return m_CurrentProgress;
}
void LoadingComponent::LoadingComplete()
{
	m_LoadingComplete = true;
}

void LoadingComponent::LoadProgress()
{
	m_CurrentProgress += 0.05f;
	if (m_CurrentProgress > 0.9f)
	{
		LoadingTimerEvent->Destory();
		m_CurrentProgress = 0.9f;
	}
}
