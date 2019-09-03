


#include "pch.h"
#include "EnginePerformance.h"
#include "GlobalLinkStream.h"
#include "Utill/Hash.h"
#pragma comment(lib, "pdh.lib")
using namespace std;
using namespace std::chrono;


EnginePerformance::EnginePerformance() : 
	m_hQuery(0), m_hTotalCounter(0)
{
	PdhOpenQuery(0, 0, &m_hQuery);
	PdhAddCounter(m_hQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &m_hTotalCounter);




	SYSTEM_INFO SystemInfo = { 0 };
	GetSystemInfo(&SystemInfo);
	if (SystemInfo.dwNumberOfProcessors > 0)
	{
		uint32_t core_count = (uint32_t)SystemInfo.dwNumberOfProcessors;
		m_hCounterPerCore.resize(core_count);

		for (uint32_t i = 0; i < core_count; ++i)
		{
			TCHAR szFullCounterPath[1024] = { 0 };
			wsprintf(szFullCounterPath, TEXT("\\Processor(%d)\\%% Processor Time"), i);
			PdhAddCounter(m_hQuery, szFullCounterPath, 1, &m_hCounterPerCore[i]);
		}
	}
}
EnginePerformance::~EnginePerformance()
{
	if (m_hQuery)
	{
		PdhCloseQuery(m_hQuery);
	}
}
void EnginePerformance::Reset()
{
	if (!m_TimerCategoryMap.empty())
	{
		m_SubmitRootNodes = m_TimerCategoryMap;
		m_TimerCategoryMap.clear();
	}
}
EnginePerformance::TimerNode* EnginePerformance::Register(
	const std::string& category, 
	const std::string& parent_name,
	const string& name)
{
	std::lock_guard<mutex> lock(m_Mutex);
	shared_ptr<TimerNode> new_node = make_shared<TimerNode>();
	TimerNode* pNode = new_node.get();
	pNode->Name = name;
	pNode->ThreadID = this_thread::get_id();
	m_TimerNameMap[name] = pNode;
	
	auto& node_map = m_TimerCategoryMap[category];
	auto iter = m_TimerNameMap.find(parent_name);

	// 소속 함수가 없을 경우
	if (iter == m_TimerNameMap.end())
	{
		new_node->Parent = nullptr;
		// 루트 노드일경우 따로 저장
		if (parent_name == "PerformanceTimer_Root")
		{
			node_map.emplace(name, new_node);
		}
		else
		{
			m_TimerCategoryMap["None"].emplace(name, new_node);
			m_TimerCategoryMap.erase(category);
		}
		return pNode;
	}

	// 있을 경우
	TimerNode* parent_node = (*iter).second;
	new_node->Parent = parent_node;
	parent_node->Childs.push_back(new_node);
	

	return pNode;
}

EnginePerformance::TimerCategoryMap EnginePerformance::GetCategoryTimerMap() const
{
	return m_SubmitRootNodes;
}

EnginePerformance::CPUInfo EnginePerformance::GetCPUInfo() const
{
	CPUInfo info;
	PDH_FMT_COUNTERVALUE total_val;
	PdhCollectQueryData(m_hQuery);
	PdhGetFormattedCounterValue(m_hTotalCounter, PDH_FMT_DOUBLE, NULL, &total_val);
	info.CoreCount = (uint32_t)m_hCounterPerCore.size();
	info.TotalUsage = total_val.doubleValue;
	info.CoreUsage.resize(info.CoreCount);
	for (uint32_t i = 0; i < info.CoreCount; ++i)
	{
		PDH_FMT_COUNTERVALUE val;
		PdhGetFormattedCounterValue(m_hCounterPerCore[i], PDH_FMT_DOUBLE, NULL, &val);
		info.CoreUsage[i] = val.doubleValue;
	}
	return info;
}

EnginePerformance::MemoryInfo EnginePerformance::GetMemInfo()
{
	MemoryInfo result;
	MEMORYSTATUSEX mem_info;
	mem_info.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&mem_info);


	result.PhyMem = mem_info.ullTotalPhys / 1024;
	result.UsedPhyMem = mem_info.ullAvailPhys / 1024;
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

	result.UsedPhyMemByMe = pmc.WorkingSetSize / 1024;
	return result;
}


// begin
PerformanceTimer::PerformanceTimer(const std::string& category, const std::string& parent_name, const std::string& function_name)
{
	EnginePerformance* EP = GlobalLinkData::_EnginePerformance.get();
	m_Node = EP->Register(category, parent_name, function_name);

	// 시간 체크 시작
	m_Start = system_clock::now();
}

PerformanceTimer::~PerformanceTimer()
{
	m_End = system_clock::now();
	duration<float> tick = duration<float>(m_End - m_Start);
	m_Node->Tick = tick.count();
}



void  ScopedTimer::Start()
{
	m_Start = system_clock::now();
}
float ScopedTimer::End()
{
	m_End = system_clock::now();
	duration<float> tick = duration<float>(m_End - m_Start);
	return tick.count();
}