#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <Pdh.h>
#include <map>
#include <PdhMsg.h>
#include <iostream>
class PerformanceTimer;
class EnginePerformance
{

	// ThreadID + 포인터
public:
	struct TimerNode
	{
		std::string Name = "None";
		std::vector<std::shared_ptr<TimerNode>> Childs;
		std::thread::id ThreadID;
		TimerNode* Parent = nullptr;
		float Tick = 0.0f;
	};
	struct CPUInfo
	{
		uint32_t CoreCount = 0;
		double TotalUsage = 0.0;
		std::vector<double> CoreUsage;
	};
	struct MemoryInfo
	{
		long long PhyMem = 0;
		long long UsedPhyMem = 0;
		long long UsedPhyMemByMe = 0;
	};
	using TimerNodeMap = std::map<std::string, std::shared_ptr<TimerNode>>;
	using TimerCategoryMap = std::map<std::string, TimerNodeMap>;
public:
	EnginePerformance();
	~EnginePerformance();
	void Reset();
	TimerNode* Register(
		const std::string& category, 
		const std::string& parent_name,
		const std::string& name);
	TimerCategoryMap GetCategoryTimerMap() const;
	CPUInfo GetCPUInfo() const;
	MemoryInfo GetMemInfo();
private:
	// 성능 시간
	TimerCategoryMap m_TimerCategoryMap;
	TimerCategoryMap m_SubmitRootNodes;
	std::map<std::string, TimerNode*> m_TimerNameMap;

	// Cpu 사용률
	PDH_HQUERY   m_hQuery;
	PDH_HCOUNTER m_hTotalCounter;
	std::vector<PDH_HCOUNTER> m_hCounterPerCore;


	std::mutex m_Mutex;
};

class PerformanceTimer
{
	friend EnginePerformance;
public:
	PerformanceTimer(const std::string& category,const std::string& parent_name, const std::string& function_name);
	~PerformanceTimer();
private:
	EnginePerformance::TimerNode* m_Node;
	std::chrono::system_clock::time_point m_Start;
	std::chrono::system_clock::time_point m_End;
};

class ScopedTimer
{
public:
	void  Start();
	float End();
private:
	std::chrono::system_clock::time_point m_Start;
	std::chrono::system_clock::time_point m_End;
};


//#define ENGINE_PERFORMANCE_TIMER_START(root_name) PerformanceTimer performance_timer("JGEngine", "PerformanceTimer_Root", root_name)
//
//#define ENGINE_PERFORMANCE_TIMER(parent_name, timer_name) PerformanceTimer performance_timer("JGEngine", parent_name, timer_name)  
//
//#define ROOT_PERFORMANCE_TIMER(category, timer_name) PerformanceTimer performance_timer(category, "PerformanceTimer_Root", timer_name) 
//
//#define CHILD_PERFORMANCE_TIMER(category, parent_name,timer_name) PerformanceTimer performance_timer(category, parent_name, timer_name)
//
//#define DEFAULT_PERFORMANCE_TIMER(timer_name) PerformanceTimer performance_timer("", "", timer_name)

