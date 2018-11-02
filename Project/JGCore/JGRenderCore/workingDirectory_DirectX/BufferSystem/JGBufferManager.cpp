#include"JGBufferManager.h"
#include"DirectX/DirectX.h"
#include"BufferSystem/JGBuffer.h"
using namespace JGRC;
using namespace std;
JGBufferManager* JGBufferManager::Instance = nullptr;
JGBufferManager::JGBufferManager() {}
JGBufferManager::~JGBufferManager() {}
JGBufferManager* JGBufferManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new JGBufferManager;
	}
	return Instance;
}
void JGBufferManager::Release()
{
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}
JGBuffer* JGBufferManager::CreateBuffer(const EBufferType BufferType, const EUsageType UsageType, const ECPUType CpuType,  void* data, const uint DataSize, const uint DataCount)
{
	JGBuffer* resultBuffer = nullptr;
	unique_ptr<JGBuffer> buffer = make_unique<JGBuffer>();
	bool result = buffer->CreateBuffer(BufferType, UsageType, CpuType, data, DataSize * DataCount);
	if (!result)
	{
		JGLOG(log_Error, "JGRC::JGBufferManager", "Failed Create JGBuffer");
		return resultBuffer;
	}
	resultBuffer = buffer.get();

	m_vBuffers.push_back(move(buffer));
	return resultBuffer;
}

void JGBufferManager::DeleteBuffer(const JGBuffer* buffer)
{
	remove_if(m_vBuffers.begin(), m_vBuffers.end(),
		[buffer](const shared_ptr<JGBuffer>& pBuffer)
	{
		if (pBuffer.get() == buffer)
		{
			return true;
		}
		return false;
	});
}