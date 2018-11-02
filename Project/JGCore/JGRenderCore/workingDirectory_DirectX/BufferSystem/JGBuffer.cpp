#include"JGBuffer.h"
#include"DirectX/DirectX.h"
#include"DirectX/JGDevice.h"
using namespace JGRC;
DirectX* JGBuffer::m_pDirectX = nullptr;

JGBuffer::JGBuffer()  {}
JGBuffer::~JGBuffer() {}

bool JGBuffer::CreateBuffer(EBufferType bufferType, EUsageType usage, ECPUType CPUtype,
	void* data, size_t size)
{
	m_pDirectX = DirectX::GetInstance();
	m_BufferType = bufferType;
	m_BindDataSize = size;
	HRESULT result = S_OK;
	D3D11_SUBRESOURCE_DATA pSourceData;

	// 버퍼 타입에 따른 바인드 플래그
	switch (bufferType)
	{
	case EBufferType::ConstantBuffer:
		m_BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		break;
	case EBufferType::VertexBuffer:
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		break;
	case EBufferType::IndexBuffer:
		m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		break;
	case EBufferType::InstanceBuffer:
		m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		break;
	}
	// 용도 타입
	switch (usage)
	{
	case EUsageType::Static:
		m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
		break;
	case EUsageType::Dynamic:
		m_BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		break;
	}
	// CPU 작성 타입
	switch (CPUtype)
	{
	case ECPUType::Access_Write:
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case ECPUType::Access_Read:
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		break;
	case ECPUType::None:
		m_BufferDesc.CPUAccessFlags = 0;
		break;
	}
	m_BufferDesc.ByteWidth = (UINT)size;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;

	if (bufferType != EBufferType::ConstantBuffer)
	{
		// 데이터 저장
		pSourceData.pSysMem = data;
		pSourceData.SysMemPitch = 0;
		pSourceData.SysMemSlicePitch = 0;
		result = m_pDirectX->GetDevice()->CreateBuffer(&m_BufferDesc, &pSourceData, m_Buffer.GetAddressOf());
		if (FAILED(result))
		{
			JGLOG(log_Error, "JGRC::JGBuffer", "Failed Create JGBuffer");
			return false;
		}
	}
	else
	{
		result = m_pDirectX->GetDevice()->CreateBuffer(&m_BufferDesc, nullptr , m_Buffer.GetAddressOf());
		if (FAILED(result))
		{
			JGLOG(log_Error, "JGRC::JGBuffer", "Failed Create JGBuffer");
			return false;
		}
	}
	return true;
}

bool JGBuffer::Write(EMapType type, void* InData)
{
	// 타입 체크
	if (type == EMapType::Read)
	{
		JGLOG(log_Error, "JGRC::JGBuffer", "type is EMapType::Read but Try Write JGBuffer");
		return false;
	}
	// CPU 플래그 체크
	if (m_BufferDesc.CPUAccessFlags != D3D11_CPU_ACCESS_WRITE)
	{
		JGLOG(log_Error, "JGRC::JGBuffer", "type is not ECPUType::Access_Write but Try Write JGBuffer");
		return false;
	}
	// 호환성 체크
	if (type == EMapType::Write_Discard && m_BufferDesc.Usage != D3D11_USAGE_DYNAMIC)
	{
		JGLOG(log_Error, "JGRC::JGBuffer", "JGBuffer's type is EMapType::Write_Discard but UsageType is not UsageType::Dynamic");
		return false;
	}
	if (type == EMapType::Write_NoOverWrite && m_BufferType != EBufferType::ConstantBuffer)
	{
		JGLOG(log_Error, "JGRC::JGBuffer", "JGBuffer's type is EMapType::Write_NoOverWrite but UsageType is not ConstantBuffer");
		return false;
	}
	HRESULT result = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAP MapFlag;

	switch (type)
	{
	case EMapType::Read:
		MapFlag = D3D11_MAP_READ;
		break;
	case EMapType::Write:
		MapFlag = D3D11_MAP_WRITE;
		break;
	case EMapType::Read_Write:
		MapFlag = D3D11_MAP_READ_WRITE;
		break;
	case EMapType::Write_Discard:
		MapFlag = D3D11_MAP_WRITE_DISCARD;
		break;
	case EMapType::Write_NoOverWrite:
		MapFlag = D3D11_MAP_WRITE_NO_OVERWRITE;
		break;
	}

	// 락걸고 데이터 삽입..
	result = m_pDirectX->GetContext()->Map(m_Buffer.Get(), 0, MapFlag, 0, &mappedResource);
	if (FAILED(result))
	{
		JGLOG(log_Error, "JGRC::JGBuffer", "Falied try JGBuffer Lock");
		return false;
	}
	void* DataPtr = (void*)mappedResource.pData;
	memcpy(DataPtr, InData, m_BindDataSize);
	// 락 해제
	m_pDirectX->GetContext()->Unmap(m_Buffer.Get(), 0);
	return true;
}
bool JGBuffer::Read(EMapType type, void* OutData)
{
	// 미구현
	JGLOG(log_Warning, "JGRC::JGBuffer", "JGBuffer::Read is not implemented");
	return true;
}
ID3D11Buffer* JGBuffer::Get()
{
	if (m_Buffer.Get())
	{
		return m_Buffer.Get();
	}
	JGLOG(log_Error, "JGRC::JGBuffer", "JGBuffer is nullptr");
	return nullptr;
}
ID3D11Buffer** JGBuffer::GetAddress()
{
	if (m_Buffer.Get())
	{
		return m_Buffer.GetAddressOf();
	}
	JGLOG(log_Error, "JGRC::JGBuffer", "JGBuffer is nullptr");
	return nullptr;
}
EBufferType JGBuffer::GetType()
{
	return m_BufferType;
}
void JGBuffer::GetDesc(D3D11_BUFFER_DESC& Desc)
{
	Desc = m_BufferDesc;
}
size_t JGBuffer::GetBindingDataSize()
{
	return m_BindDataSize;
}