#pragma once
#include"Engine.h"


class JTimerHandle;
typedef std::function<void()>        TimerEvent;
typedef std::chrono::system_clock    Clock;
typedef std::chrono::duration<float> Duration;

/*
Name : EHandleType
@m ERepeat  : 정해진 카운트만큼 반복한다.
@m EDefault : 한번 실행한다.
*/
enum class ENGINE_EXPORT EHandleType
{
	ERepeat, EDefault
};

/*
struct : TimerHandleDesc
@m TimerEvent   Event      : 실행할 이벤트(void() 형 함수)
@m EHandleType HandleType  : 실행 타입
@m float        Delay      : 처음 실행할떄 지연 시간
@m float        GapTime    : 반복 실행할떄 사이 시간
@m size_t       Count      : 반복 횟수
*/
typedef struct ENGINE_EXPORT TimerHandleDesc
{
	TimerEvent   Event;
	EHandleType HandleType;

	float Delay = 0.0f;
	float GapTime = 0.0f;
	size_t Count = 0;
}TimerHandleDesc;


/*
Class : JMainTimer
@m Clock::time_point m_TimerTime  : 시간 측정 도구
@m float             m_DeltaTime  : 1 프레임당 시간
@m float             m_ActiveTime : 타이머가 활성화되어있었던 시간
@m float             m_StopTime   : 타이머가 멈춰있었던 시간
@m bool              m_bStop      : 정지 여부
*/
class ENGINE_EXPORT JMainTimer
{
private:
	JMainTimer() {}
	JMainTimer(const JMainTimer& copy) {};
	~JMainTimer() {}
private:
	Clock::time_point   m_TimerTime;

	float m_DeltaTime   = 0.0f;
	float m_ActiveTime  = 0.0f;
	float m_StopTime    = 0.0f;
	bool  m_bStop       = false;

public:
	/*
	Exp : JMainTimer의 인스턴스를 취득한다. */
	static JMainTimer* GetInstance();
	/*
	Exp : JMainTimer의 인스턴스를 파괴한다. */
	static void Destroy();
	/*
	Exp : 타이머 시작 */
	void  Start();
	/*
	Exp : 타이머 정지 */
	void  Stop();
	/*
	Exp : 1 프레임 ( 1 프레임 주기로 타이머 업데이트 ) */
	void  Tick();

	/*
	Exp : 1 프레임당 차이 시간  */
	const float GetDeltaTime();
	/*
	Exp : 메인 타이머가 활동한 시간*/
	const float GetActiveTime();
	/*
	Exp : 메인 타이머가 멈췃던 시간*/
	const float GetStopTime();
	/*
	Exp : 총 시간 */
	const float GetTotalTime();


};

/*
Static Class : JTimerEventManager
@m std::forward_list<JTimerHandle*> m_flTimerHandles : 타이머 핸들 배열 리스트
*/
class ENGINE_EXPORT JTimerEventManager
{
private:
	static std::forward_list<JTimerHandle*> m_flTimerHandles;
public:
	/*
	Exp : 1프레임마다 호출되는 함수 
	@param const float DeltaTime : JMainTimer의 1프레임당 시간 */
	static void Tick(const float DeltaTime);
	/*
	Exp : 타이머 이벤트를 생성한다.
	@param JTimerHandle** handle            : 타이머 이벤트 정보를 가지고있는 Handle클래스 주소
	@param const          TimerEvent& Event : void()형 함수 이벤트
	@param EHandleType    HandleType        : 핸들 타입( 한번 호출, 반복 호출 등등..)
	@param const float    Delay             : 최초 시작 딜레이 시간
	@param const float    GapTime           : 이벤트 반복 호출 사이 딜레이 시간
	@param const size_t   Count             : 이벤트 반복 횟수
	*/
	static void CreateTimerEvent(JTimerHandle** handle,const TimerEvent& Event, EHandleType HandleType,
		const float Delay, const float GapTime, const size_t Count);
private:
	static void EventUpdate(const float DeltaTime);
};

/*
Class : JTimerHandle
@m 	TimerHandleDesc m_HandleDesc : 타이머 핸들 정보 목록
@m	bool  m_bInit                : 초기화 여부
@m	bool  m_bEnd                 : 이벤트 종료 여부
@m	bool  m_bDestroy             : 타이머 핸들 파괴 여부
*/
class ENGINE_EXPORT JTimerHandle
{
private:
	friend class JTimerEventManager;
private:
	JTimerHandle() {}
	~JTimerHandle() { Destory(); }
	TimerHandleDesc m_HandleDesc;
	bool  m_bInit    = false;
	bool  m_bEnd     = false;
	bool  m_bDestroy = false;

	// 딜레이 계산을 위한 더미 변수
	float DummyTime = 0.0f;
public:
	/*
	Exp : 이벤트 종료 여부*/
	bool IsEnd();
	/*
	Exp : 타이머 핸들 파괴 여부*/
	bool IsDestory();
	/*
	Exp : 타이머 핸들 파괴*/
	void Destory();
};
