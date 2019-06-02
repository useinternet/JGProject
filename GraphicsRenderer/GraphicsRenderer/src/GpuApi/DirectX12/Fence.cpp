#include"pch.h"
#include"Fence.h"

namespace GR
{
	namespace Dx12
	{
		using namespace std::chrono;


		Fence::Fence() :
			m_FenceValue(0)
		{
			m_FenceEvent = ::CreateEvent(nullptr, false, false, nullptr);
			assert(m_FenceEvent && "Failed to create fence event.");
		}
		Fence::~Fence()
		{
			::CloseHandle(m_FenceEvent);
		}
		void Fence::Initialize(ComPtr<ID3D12Device> device)
		{
			HRESULT hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_D3D_Fence));
			assert(SUCCEEDED(hr) && "failed Create Fence Object");
		}
		void Fence::WaitForFenceValue(uint64_t value, milliseconds duration)
		{
			std::lock_guard<std::mutex> lock(m_EventMutex);

			if (m_D3D_Fence->GetCompletedValue() < value)
			{
				HRESULT hr = m_D3D_Fence->SetEventOnCompletion(value, m_FenceEvent);
				assert(SUCCEEDED(hr) && "failed SetEventOnCompletion in Fence Object");
				::WaitForSingleObject(m_FenceEvent, static_cast<DWORD>(duration.count()));
			}
		}

	}
}