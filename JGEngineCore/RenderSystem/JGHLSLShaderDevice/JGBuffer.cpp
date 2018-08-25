#include"JGBuffer.h"
#include"../JGDeviceD.h"
#include"../../EngineStatics/JGLog.h"


JGBuffer::JGBuffer()  {}
JGBuffer::~JGBuffer() {}

bool JGBuffer::CreateBuffer(JGDeviceD* Device, JGBufferType bufferType, JGUsageType usage, JGCPUType CPUtype,
	void* data, UINT size)
{
	m_pDevice = Device;
	m_BufferType = bufferType;

	HRESULT result = S_OK;
	D3D11_SUBRESOURCE_DATA pSourceData;

	// 버퍼 타입에 따른 바인드 플래그
	switch (bufferType)
	{
	case JGBufferType::ConstantBuffer:
		m_BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		break;
	case JGBufferType::VertexBuffer:
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		break;
	case JGBufferType::IndexBuffer:
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		break;
	case JGBufferType::InstanceBuffer:
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		break;
	}
	// 용도 타입
	switch (usage)
	{
	case JGUsageType::Static:
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		break;
	case JGUsageType::Dynamic:
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		break;
	}

	// CPU 작성 타입
	switch (CPUtype)
	{
	case JGCPUType::Access_Write:
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case JGCPUType::Access_Read:
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		break;
	case JGCPUType::None:
		m_BufferDesc.CPUAccessFlags = 0;
		break;
	}
	m_BufferDesc.ByteWidth = size;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	// 데이터 저장
	pSourceData.pSysMem = data;
	pSourceData.SysMemPitch = 0;
	pSourceData.SysMemSlicePitch = 0;

	result = m_pDevice->GetDevice()->CreateBuffer(&m_BufferDesc, &pSourceData, m_Buffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool JGBuffer::Write(JGMapType type, void* InData)
{
	// 타입 체크
	if (type == JGMapType::Read)
	{
		JGLog::Write(ELogLevel::Warning, TT(
			"The JGMapType of JGBuffer is a read function, but it is a write function."));
		return false;
	}
	// CPU 플래그 체크
	if (m_BufferDesc.CPUAccessFlags != D3D11_CPU_ACCESS_WRITE)
	{
		JGLog::Write(ELogLevel::Warning, TT(
			"The JGCPUType of JGBuffer is not write function."));
		return false;
	}
	// 호환성 체크
	if (type == JGMapType::Write_Discard && m_BufferDesc.Usage != D3D11_USAGE_DYNAMIC)
	{
		JGLog::Write(ELogLevel::Warning, TT(
			"JGMapType is Write_Discard, but the UsageType in the current buffer list is not Dynamic."));
		return false;
	}
	if (type == JGMapType::Write_NoOverWrite && m_BufferType != JGBufferType::ConstantBuffer)
	{
		JGLog::Write(ELogLevel::Warning, TT(
			"JGMapType is Write_NoOverWrite, but the JGBufferType is not ContantBuffer."));
		return false;
	}
	HRESULT result = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAP MapFlag;

	switch (type)
	{
	case JGMapType::Read:
		MapFlag = D3D11_MAP_READ;
		break;
	case JGMapType::Write:
		MapFlag = D3D11_MAP_WRITE;
		break;
	case JGMapType::Read_Write:
		MapFlag = D3D11_MAP_READ_WRITE;
		break;
	case JGMapType::Write_Discard:
		MapFlag = D3D11_MAP_WRITE_DISCARD;
		break;
	case JGMapType::Write_NoOverWrite:
		MapFlag = D3D11_MAP_WRITE_NO_OVERWRITE;
		break;
	}


	result = m_pDevice->GetContext()->Map(
		m_Buffer.Get(), 0, MapFlag, 0, &mappedResource
	);
	if (FAILED(result))
	{
		return false;
	}
	mappedResource.pData = InData;

	m_pDevice->GetContext()->Unmap(m_Buffer.Get(), 0);

	return true;
}
bool JGBuffer::Read(JGMapType type, void * OutData)
{
	// 미구현
	return true;
}
ID3D11Buffer* JGBuffer::Get()
{
	if (m_Buffer.Get())
	{
		return m_Buffer.Get();
	}
	JGLog::Write(ELogLevel::Warning, TT(
		"Bubber is not exist."));
	return nullptr;
}
ID3D11Buffer** JGBuffer::GetAddress()
{
	if (m_Buffer.Get())
	{
		return m_Buffer.GetAddressOf();
	}
	JGLog::Write(ELogLevel::Warning, TT(
		"Bubber is not exist."));
	return nullptr;
}
JGBufferType JGBuffer::GetType()
{
	return m_BufferType;
}
void JGBuffer::GetDesc(D3D11_BUFFER_DESC& Desc)
{
	Desc = m_BufferDesc;
}