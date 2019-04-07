#include<PCH.h>
#include"Fence.h"
#include"DirectXCore.h"
using namespace Dx12;
using namespace std::chrono;


Fence::Fence() : 
	m_FenceValue(0)
{
	ThrowIfFailed(DxDevice::GetDevice()->CreateFence(
		0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_D3D_Fence)));

	m_FenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
	assert(m_FenceEvent && "Failed to create fence event.");


}
Fence::~Fence()
{
	::CloseHandle(m_FenceEvent);
}
void Fence::WaitForFenceValue(uint64_t value, milliseconds duration)
{
	if (m_D3D_Fence->GetCompletedValue() < value)
	{
		ThrowIfFailed(m_D3D_Fence->SetEventOnCompletion(value, m_FenceEvent));
		::WaitForSingleObject(m_FenceEvent, static_cast<DWORD>(duration.count()));
	}
}