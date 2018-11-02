#pragma once
#include"common.h"
#include<thread>
#include<functional>
#include<vector>
#include<atomic>
#include<mutex>
#include<queue>
#include<condition_variable>
/*
Static Class : JThreadManager
@Class ThreadSafe_Que : 다중 쓰레드에 호환되는 queue 클래스

@m const unsigned short     WorkerCount  : 동작하는 쓰레드 갯수
@m ThreadSafe_Que<Business> m_Businesses : 작업 함수 큐
@m std::vector<std::thread> m_vWorker    : 동작 쓰레드 배열
@m std::atomic<bool>        m_IsWorking  : 클래스 동작 여부
*/
class  JThreadManager
{
private:
	/*
	Class : ThreadSafe_Que
	@m  std::queue<Task>        m_TaskQue : 함수 큐
	@m	std::mutex              m_Mutex   : 뮤텍스(락);
	@m	std::condition_variable m_cdVar   : 조건 변수
	*/
	template< typename Task >
	class ThreadSafe_Que
	{
	private:
		friend JThreadManager;
	private:
		std::queue<Task>        m_TaskQue;
		std::mutex              m_Mutex;
		std::condition_variable m_cdVar;
	public:
		void  push(const Task& task);
		bool  pop(Task& task);
	};
	typedef std::function<void()> Business;
private:
	static const unsigned short WorkerCount;
private:
	static ThreadSafe_Que<Business> m_Businesses;
	static std::vector<std::thread> m_vWorker;
	static std::atomic<bool>        m_IsWorking;

private:
	JThreadManager() {}
	~JThreadManager() {}
	static void Init();

	static void Work();
public:
	/*
	Exp : JThreadManager의 인스턴스를 취득한다. */
	static JThreadManager* GetInstance();
	/*
	Exp : JThreadManager의 인스턴스를 파괴한다. */
	static void Destroy();
	/*
	Exp : 작업 함수 큐에 함수를 추가한다. 
	@param const Business& Task : 추가할 작업 함수 */
	static void AddTask(const Business& Task);
};

