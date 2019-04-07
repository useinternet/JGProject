#include<PCH.h>
#include"UploadBuffer.h"
#include"DirectXCore.h"
using namespace Dx12;
using namespace std;

UploadBuffer::Page::Page(size_t sizeInBytes) :
	m_PageSize(sizeInBytes),
	m_Offset(0),
	m_CPUptr(nullptr),
	m_GPUptr(D3D12_GPU_VIRTUAL_ADDRESS(0))
{
	auto Device = DxDevice::GetDevice();

	ThrowIfFailed(Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(m_PageSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_D3D12Resource.GetAddressOf())));

	m_GPUptr = m_D3D12Resource->GetGPUVirtualAddress();
	m_D3D12Resource->Map(0, nullptr, &m_CPUptr);
}
UploadBuffer::Page::~Page()
{
	m_D3D12Resource->Unmap(0, nullptr);
	m_CPUptr = nullptr;
	m_GPUptr = D3D12_GPU_VIRTUAL_ADDRESS(0);
}

bool UploadBuffer::Page::HasSpace(size_t sizeInBytes, size_t alignment) const
{
	size_t alignedSize   = Math::AlignUp(sizeInBytes, alignment);
	size_t alignedOffset = Math::AlignUp(m_Offset, alignment);

	return alignedOffset + alignedSize <= m_PageSize;
}

UploadBuffer::Allocation UploadBuffer::Page::Allocate(size_t sizeInBytes, size_t alignment)
{
	if (!HasSpace(sizeInBytes, alignment))
	{
		throw std::bad_alloc();
	}

	size_t alignedSize = Math::AlignUp(sizeInBytes, alignment);
	m_Offset = Math::AlignUp(m_Offset, alignment);

	Allocation allocation;
	allocation.CPU = static_cast<uint8_t*>(m_CPUptr) + m_Offset;
	allocation.GPU = m_GPUptr + m_Offset;

	m_Offset += alignedSize;

	return allocation;
}
void UploadBuffer::Page::Reset()
{
	m_Offset = 0;
}


UploadBuffer::UploadBuffer(size_t pageSize) : m_PageSize(pageSize) { }
UploadBuffer::~UploadBuffer() {}

UploadBuffer::Allocation UploadBuffer::Allocate(size_t sizeInBytes, size_t alignment)
{
	if (sizeInBytes > m_PageSize)
	{
		throw std::bad_alloc();
	}

	if (!m_CurrentPage || !m_CurrentPage->HasSpace(sizeInBytes, alignment))
	{
		m_CurrentPage = RequestPage();
	}

	return m_CurrentPage->Allocate(sizeInBytes, alignment);
}
void UploadBuffer::Reset()
{
	m_CurrentPage = nullptr;
	m_AvailablePages = m_PagePool;

	for (auto& page : m_AvailablePages)
	{
		page->Reset();
	}
}
shared_ptr<UploadBuffer::Page> UploadBuffer::RequestPage()
{
	std::shared_ptr<Page> page;

	if (!m_AvailablePages.empty())
	{
		page = m_AvailablePages.front();
		m_AvailablePages.pop_front();
	}
	else
	{
		page = make_shared<Page>(m_PageSize);
		m_PagePool.push_back(page);
	}
	return page;
}