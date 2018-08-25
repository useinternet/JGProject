#include"JGBufferManager.h"
#include"../JGDeviceD.h"
#include"JGBuffer.h"
#include"../../EngineStatics/JGLog.h"
using namespace std;

JGBufferManager::JGBufferManager() {}
JGBufferManager::~JGBufferManager() {}


void JGBufferManager::InitBufferManager(JGDeviceD* Device)
{
	m_pDevice = Device;
}
bool JGBufferManager::AddBuffer(const wstring& name, JGBufferType bufferType, JGUsageType usage, JGCPUType CPUtype,
	void* data, UINT size)
{
	bool result = true;
	unique_ptr<JGBuffer> buffer = make_unique<JGBuffer>();
	result = buffer->CreateBuffer(m_pDevice, bufferType, usage, 
		CPUtype, data, size);

	if (!result)
	{
		JGLog::Write(ELogLevel::Warning, TT("Create JGBuffer of %s Failed."), name.c_str());
		return false;
	}
	// »ðÀÔ
	m_JGBuffers.insert(pair<const wstring, unique_ptr<JGBuffer>>(name, move(buffer)));
	return true;
}
bool JGBufferManager::Write(const wstring& name, JGMapType type, void* InData)
{
#ifdef _DEBUG
	auto iter = m_JGBuffers.find(name);
	if (iter == m_JGBuffers.end())
	{
		JGLog::Write(ELogLevel::Warning, TT("%s is not exist in JGBufferManager"), name.c_str());
		return false;
	}
#endif
	bool result = m_JGBuffers[name]->Write(type, InData);
	if (!result)
	{
		JGLog::Write(ELogLevel::Warning, TT("Write JGBuffer of %s Failed"), name.c_str());
		return false;
	}
	return true;
}
JGBuffer* JGBufferManager::GetBuffer(const std::wstring& name)
{
#ifdef _DEBUG
	auto iter = m_JGBuffers.find(name);
	if (iter == m_JGBuffers.end())
	{
		JGLog::Write(ELogLevel::Warning, TT("%s is not exist in JGBufferManager"), name.c_str());
		return false;
	}
#endif
	return m_JGBuffers[name].get();
}