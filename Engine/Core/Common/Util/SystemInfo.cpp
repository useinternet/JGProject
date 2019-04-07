#include<PCH.h>
#include"SystemInfo.h"
#include<Pdh.h>
#include<PdhMsg.h>
#include<Psapi.h>

#pragma comment(lib, "Pdh.lib")
namespace Common
{
	namespace Util
	{

		SystemInfo::SystemInfo()
		{
			PDH_STATUS status;
			canReadCpu = true;

			status = PdhOpenQuery(NULL, 0, &queryHandle);

			if (status != ERROR_SUCCESS) {
				canReadCpu = false;
			}
			status = PdhAddCounter(queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &counterHandle);
			if (status != ERROR_SUCCESS) {
				canReadCpu = false;
			}
			lastSampleTime = GetTickCount();
			cpuUsage = 0;

			SYSTEM_INFO systemInfo;
			GetSystemInfo(&systemInfo);
			CpuCoreNum = systemInfo.dwNumberOfProcessors;
		}
		SystemInfo::~SystemInfo()
		{
			//if (canReadCpu) { PdhCloseQuery(queryHandle); }
		}

		void SystemInfo::NewFrame()
		{
			MEMORYSTATUSEX ms;
			PROCESS_MEMORY_COUNTERS_EX pmc;
			ms.dwLength = sizeof(MEMORYSTATUSEX);
			GlobalMemoryStatusEx(&ms);

			GetProcessMemoryInfo(
				GetCurrentProcess(),
				reinterpret_cast<PPROCESS_MEMORY_COUNTERS>(&pmc),
				sizeof(pmc));

			virtualTotalMem = ms.ullTotalPageFile;
			virtualTotalMemUsed = ms.ullTotalPageFile - ms.ullAvailPageFile;
			virtualMemUsedByme = pmc.PrivateUsage;
			totalPhysMem = ms.ullTotalPhys;
			totalPhysMemUsed = ms.ullTotalPhys - ms.ullAvailPhys;
			PhysMemUsedByme = pmc.WorkingSetSize;


			PDH_FMT_COUNTERVALUE value;
			if (canReadCpu)
			{
				if ((lastSampleTime + 1000) < GetTickCount())
				{
					lastSampleTime = GetTickCount();
					PdhCollectQueryData(queryHandle);
					PdhGetFormattedCounterValue(
						counterHandle,
						PDH_FMT_LONG, NULL,
						&value);

					cpuUsage = value.longValue;
				}
			}
		}

	}
}