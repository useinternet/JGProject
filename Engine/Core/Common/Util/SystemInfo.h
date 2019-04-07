#pragma once
#include"Common.h"

namespace Common
{
	namespace Util
	{
		class SystemInfo
		{
		private:
			bool          canReadCpu;
			HANDLE        queryHandle;
			HANDLE      counterHandle;
			unsigned long lastSampleTime;
			long          cpuUsage;
			int CpuCoreNum;
		public:
			size_t virtualTotalMem;
			size_t virtualTotalMemUsed;
			size_t virtualMemUsedByme;
			size_t totalPhysMem;
			size_t totalPhysMemUsed;
			size_t PhysMemUsedByme;
		public:
			SystemInfo();
			~SystemInfo();
			void NewFrame();
			int GetCpuPercentage() const
			{
				int usage;
				if (canReadCpu) 
				{ 
					usage = (int)cpuUsage;
				}
				else {
					usage = 0; 
				} 
				return usage;
			}
			int GetCpuCoreNum() const {
				return CpuCoreNum;
			}
		};
	}
}