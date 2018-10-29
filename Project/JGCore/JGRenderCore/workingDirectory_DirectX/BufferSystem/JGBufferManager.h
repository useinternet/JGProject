#pragma once
#include"Common/JGRCCommon.h"
#include"JGBufferEnum.h"

namespace JGRC
{
	class JGBuffer;
	class CORE_EXPORT JGBufferManager
	{
	private:
		static JGBufferManager* Instance;
	private:
		std::vector<std::shared_ptr<JGBuffer>> m_vBuffers;
	private:
		JGBufferManager();
		~JGBufferManager();
		JGBufferManager(const JGBufferManager& copy) = delete;
		JGBufferManager& operator=(const JGBufferManager& JM) = delete;
	public:
		static JGBufferManager* GetInstance();
		static void Release();
	public:
		JGBuffer* CreateBuffer(const EBufferType BufferType, const EUsageType UsageType, const ECPUType CpuType, void* data,const uint DataSize,const uint DataCount );
		void DeleteBuffer(const JGBuffer* buffer);
	};
}
