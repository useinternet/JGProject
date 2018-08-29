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
bool JGBufferManager::AddBuffer(const wstring& name, EJGBufferType bufferType, EJGUsageType usage, EJGCPUType CPUtype,
	void* data, size_t size)
{
	bool result = true;

	if (!DuplicateCheck(name))
	{
		return true;
	}

	unique_ptr<JGBuffer> buffer = make_unique<JGBuffer>();
	result = buffer->CreateBuffer(m_pDevice, bufferType, usage, 
		CPUtype, data, size);

	if (!result)
	{
		JGLog::Write(ELogLevel::Warning, TT("Create JGBuffer of %s Failed."), name.c_str());
		return false;
	}
	// 삽입
	m_JGBuffers.insert(pair<const wstring, unique_ptr<JGBuffer>>(name, move(buffer)));
	return true;
}
bool JGBufferManager::Write(const wstring& name, EJGMapType type, void* InData)
{
#ifdef _DEBUG
	if (DuplicateCheck(name))
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
	if (DuplicateCheck(name))
	{
		JGLog::Write(ELogLevel::Warning, TT("%s is not exist in JGBufferManager"), name.c_str());
		return false;
	}
#endif
	return m_JGBuffers[name].get();
}

size_t JGBufferManager::GetBindingDataSize(const std::wstring & name)
{
#ifdef _DEBUG
	if (DuplicateCheck(name))
	{
		JGLog::Write(ELogLevel::Warning, TT("%s is not exist in JGBufferManager"), name.c_str());
		return 0;
	}
#endif
	return m_JGBuffers[name]->GetBindingDataSize();
}

bool JGBufferManager::DuplicateCheck(const std::wstring& name)
{
	// 비어져있으면 그냥 리턴
	if (m_JGBuffers.size() == 0)
	{
		return true;
	}
	// 그게아니면 체크
	auto iter = m_JGBuffers.find(name);
	if (iter == m_JGBuffers.end())
	{
		return true;
	}
	return false;
}


