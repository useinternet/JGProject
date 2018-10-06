#pragma once
#include"Engine.h"


class JTimerHandle;
typedef std::function<void()>        TimerEvent;
typedef std::chrono::system_clock    Clock;
typedef std::chrono::duration<float> Duration;

/*
Name : EHandleType
@m ERepeat  : Á¤ÇØÁø Ä«¿îÆ®¸¸Å­ ¹İº¹ÇÑ´Ù.
@m EDefault : ÇÑ¹ø ½ÇÇàÇÑ´Ù.
*/
enum class ENGINE_EXPORT EHandleType
{
	ERepeat, EDefault
};

/*
struct : TimerHandleDesc
@m TimerEvent   Event      : ½ÇÇàÇÒ ÀÌº¥Æ®(void() Çü ÇÔ¼ö)
@m EHandleType HandleType  : ½ÇÇà Å¸ÀÔ
@m float        Delay      : Ã³À½ ½ÇÇàÇÒ‹š Áö¿¬ ½Ã°£
@m float        GapTime    : ¹İº¹ ½ÇÇàÇÒ‹š »çÀÌ ½Ã°£
@m size_t       Count      : ¹İº¹ È½¼ö
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
@m Clock::time_point m_TimerTime  : ½Ã°£ ÃøÁ¤ µµ±¸
@m float             m_DeltaTime  : 1 ÇÁ·¹ÀÓ´ç ½Ã°£
@m float             m_ActiveTime : Å¸ÀÌ¸Ó°¡ È°¼ºÈ­µÇ¾îÀÖ¾ú´ø ½Ã°£
@m float             m_StopTime   : Å¸ÀÌ¸Ó°¡ ¸ØÃçÀÖ¾ú´ø ½Ã°£
@m bool              m_bStop      : Á¤Áö ¿©ºÎ
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
	Exp : JMainTimerÀÇ ÀÎ½ºÅÏ½º¸¦ ÃëµæÇÑ´Ù. */
	static JMainTimer* GetInstance();
	/*
	Exp : JMainTimerÀÇ ÀÎ½ºÅÏ½º¸¦ ÆÄ±«ÇÑ´Ù. */
	static void Destroy();
	/*
	Exp : Å¸ÀÌ¸Ó ½ÃÀÛ */
	void  Start();
	/*
	Exp : Å¸ÀÌ¸Ó Á¤Áö */
	void  Stop();
	/*
	Exp : 1 ÇÁ·¹ÀÓ ( 1 ÇÁ·¹ÀÓ ÁÖ±â·Î Å¸ÀÌ¸Ó ¾÷µ¥ÀÌÆ® ) */
	void  Tick();

	/*
	Exp : 1 ÇÁ·¹ÀÓ´ç Â÷ÀÌ ½Ã°£  */
	const float GetDeltaTime();
	/*
	Exp : ¸ŞÀÎ Å¸ÀÌ¸Ó°¡ È°µ¿ÇÑ ½Ã°£*/
	const float GetActiveTime();
	/*
	Exp : ¸ŞÀÎ Å¸ÀÌ¸Ó°¡ ¸Ø­Ÿ´ø ½Ã°£*/
	const float GetStopTime();
	/*
	Exp : ÃÑ ½Ã°£ */
	const float GetTotalTime();


};

/*
Static Class : JTimerEventManager
@m std::forward_list<JTimerHandle*> m_flTimerHandles : Å¸ÀÌ¸Ó ÇÚµé ¹è¿­ ¸®½ºÆ®
*/
class ENGINE_EXPORT JTimerEventManager
{
private:
	friend JMainTimer;
	static std::forward_list<JTimerHandle*> m_flTimerHandles;
public:
	/*
	Exp : 1ÇÁ·¹ÀÓ¸¶´Ù È£ÃâµÇ´Â ÇÔ¼ö 
	@param const float DeltaTime : JMainTimerÀÇ 1ÇÁ·¹ÀÓ´ç ½Ã°£ */
	static void Tick(const float DeltaTime);
	/*
	Exp : Å¸ÀÌ¸Ó ÀÌº¥Æ®¸¦ »ı¼ºÇÑ´Ù.
	@param JTimerHandle** handle            : Å¸ÀÌ¸Ó ÀÌº¥Æ® Á¤º¸¸¦ °¡Áö°íÀÖ´Â HandleÅ¬·¡½º ÁÖ¼Ò
	@param const          TimerEvent& Event : void()Çü ÇÔ¼ö ÀÌº¥Æ®
	@param EHandleType    HandleType        : ÇÚµé Å¸ÀÔ( ÇÑ¹ø È£Ãâ, ¹İº¹ È£Ãâ µîµî..)
	@param const float    Delay             : ÃÖÃÊ ½ÃÀÛ µô·¹ÀÌ ½Ã°£
	@param const float    GapTime           : ÀÌº¥Æ® ¹İº¹ È£Ãâ »çÀÌ µô·¹ÀÌ ½Ã°£
	@param const size_t   Count             : ÀÌº¥Æ® ¹İº¹ È½¼ö(-1ÀÌ¸é ¹«ÇÑ ¹İº¹)
	*/
	static void CreateTimerEvent(JTimerHandle** handle,const TimerEvent& Event, EHandleType HandleType,
		const float Delay, const float GapTime, const size_t Count);
private:
	static void EventUpdate(const float DeltaTime);
	static void Destroy();
};

/*
Class : JTimerHandle
@m 	TimerHandleDesc m_HandleDesc : Å¸ÀÌ¸Ó ÇÚµé Á¤º¸ ¸ñ·Ï
@m	bool  m_bInit                : ÃÊ±âÈ­ ¿©ºÎ
@m	bool  m_bEnd                 : ÀÌº¥Æ® Á¾·á ¿©ºÎ
@m	bool  m_bDestroy             : Å¸ÀÌ¸Ó ÇÚµé ÆÄ±« ¿©ºÎ
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

	// µô·¹ÀÌ °è»êÀ» À§ÇÑ ´õ¹Ì º¯¼ö
	float DummyTime = 0.0f;
public:
	/*
	Exp : ÀÌº¥Æ® Á¾·á ¿©ºÎ*/
	bool IsEnd();
	/*
	Exp : Å¸ÀÌ¸Ó ÇÚµé ÆÄ±« ¿©ºÎ*/
	bool IsDestory();
	/*
	Exp : Å¸ÀÌ¸Ó ÇÚµé ÆÄ±«*/
	void Destory();
};
