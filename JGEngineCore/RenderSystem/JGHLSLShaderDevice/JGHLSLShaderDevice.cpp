#include"JGHLSLShaderDevice.h"
#include"../JGDeviceD.h"
#include"JGBufferManager.h"
#include"../../EngineStatics/JGLog.h"
using namespace std;



JGHLSLShaderDevice::JGHLSLShaderDevice() 
{
}
JGHLSLShaderDevice::~JGHLSLShaderDevice() {}

bool JGHLSLShaderDevice::CreateDevice(JGDeviceD* Device, JGBufferManager* BufferManager)
{
	m_pDevice = Device;
	m_pBufferManager = BufferManager;
	return true;
}

void JGHLSLShaderDevice::Render(const std::wstring& name, JGShaderData* Data, JGTexture* Texture, size_t IndexCount)
{
	m_mShaders[name]->Render(Data, Texture);
	m_pDevice->GetContext()->DrawIndexed((UINT)IndexCount, 0, 0);
}

JGShader * JGHLSLShaderDevice::GetShader(const std::wstring& name)
{
	auto iter = m_mShaders.find(name);
	if (iter == m_mShaders.end())
	{
		return nullptr;
	}

	return m_mShaders[name].get();
}




