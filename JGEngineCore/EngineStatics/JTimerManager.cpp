#include"JTimerManager.h"

using namespace std::chrono;

static JMainTimer* Instance = nullptr;

//
// JTimerManager Class
//

JMainTimer* JMainTimer::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new JMainTimer();
		atexit(Destroy);
	}
	return Instance;
}
void JMainTimer::Destroy()
{
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}
void  JMainTimer::Start()
{
	m_bStop = false;
	m_TimerTime = Clock::now();
}
void  JMainTimer::Stop()
{
	m_bStop = true;
}
void  JMainTimer::Tick()
{
	Clock::time_point CurrentTime = Clock::now();


	// 타이머가 정지 됬을 경우
	if (m_bStop)
	{
		// 차이시간 : 현재시간 - 전시간(타이머가 가지고있던 시간)
		Duration deltaTime = CurrentTime - m_TimerTime;

		m_TimerTime = CurrentTime;
		m_StopTime += deltaTime.count();
		m_DeltaTime = 0.0f;
		return;
	}


	// 차이시간 : 현재시간 - 전시간(타이머가 가지고있던 시간)
	Duration deltaTime = CurrentTime - m_TimerTime;

	m_TimerTime = CurrentTime;
	m_DeltaTime = deltaTime.count();



	// 전체 흐른 시간 구하기
	m_ActiveTime += m_DeltaTime;
}
const float JMainTimer::GetDeltaTime()  { return m_DeltaTime; }
const float JMainTimer::GetActiveTime() { return m_ActiveTime; }
const float JMainTimer::GetStopTime()   { return m_StopTime; }
const float JMainTimer::GetTotalTime()  { return m_ActiveTime + m_StopTime; }

//
// JTimerEventManager Class
//
// Static Memeber Param Init
std::forward_list<JTimerHandle*> JTimerEventManager::m_flTimerHandles;
void JTimerEventManager::Tick(const float DeltaTime)
{
	// 삭제 이벤트가 발생한 타이머 핸들 삭제
	m_flTimerHandles.remove_if([](JTimerHandle* handle)->bool
	{
		if (handle->IsDestory())
		{
			return true;
		}
		return false;
	});

	// 이벤트를 업데이트한다.
	EventUpdate(DeltaTime);
}
void JTimerEventManager::CreateTimerEvent(JTimerHandle** handle, const TimerEvent& Event, EHandleType HandleType, const float Delay,
	const float GapTime, const size_t Count)
{
	// 초기화를 한번했고 아직 타이머 핸들 이벤트가 끝나지 않는 상항이라면
	// 함수를 빠져나온다.
	if ((*handle) != nullptr && (*handle)->m_bInit && !(*handle)->m_bEnd)
	{
#ifdef _DEBUG
		// 디버그 모드로 예외처리
#endif
		return;
	}
	// 처음 초기화한 핸들은 새로 생성한다.
	if ((*handle) == nullptr)
	{
		(*handle) = new JTimerHandle;
		(*handle)->m_bInit = true;
	}
	// 위 두가지 모두아니라면 핸들 이벤트함수가 모두끝나야 추가생성할수있다.
	else
	{
		(*handle)->m_bEnd = false;
	}
	(*handle)->m_HandleDesc = {
		Event,
		HandleType,
		Delay,
		GapTime,
		Count };
	//
	m_flTimerHandles.push_front((*handle));
}
void JTimerEventManager::EventUpdate(const float DeltaTime)
{
	for (auto& iter : m_flTimerHandles)
	{
		// 이벤트가 끝난 타이머 핸들은 무시한다.
		if (iter->m_bEnd)
		{
			continue;
		}
		TimerHandleDesc& desc = iter->m_HandleDesc;
		switch (desc.HandleType)
		{
		case EHandleType::EDefault:
			if (desc.Delay <= 0)
			{
				// Delay초만큼 지난 핸들은 함수 실행
				desc.Event();
				iter->m_bEnd = true;
			}
			desc.Delay -= DeltaTime;
			break;
		case EHandleType::ERepeat:
			if (desc.Count > 0 && desc.Delay <= 0 && iter->DummyTime <=0)
			{
				desc.Event();
				// 이벤트 실행 마다 카운트 감소 / 딜레이 감소
				desc.Count--;
				iter->DummyTime = desc.GapTime;
			}
			// GapTime 연산
			if (desc.Delay <= 0)
			{
				iter->DummyTime -= DeltaTime;
			}
			// 반복횟수를 다채웠을 경우 이벤트 종료
			if (desc.Count <= 0)
			{
				iter->m_bEnd = true;
			}
			desc.Delay -= DeltaTime;
			break;
		default:
			break;
		}

	}
}




//
// JTimerHandle Class
//
bool JTimerHandle::IsEnd()    { return m_bEnd; }
bool JTimerHandle::IsDestory(){ return m_bDestroy; }
void JTimerHandle::Destory()  { m_bDestroy = true; }

