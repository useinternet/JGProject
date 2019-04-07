#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include<atomic>
namespace Dx12
{
	class Fence
	{
	private:
		ComPtr<ID3D12Fence> m_D3D_Fence;
		std::atomic_uint64_t m_FenceValue;
		HANDLE   m_FenceEvent;
	public:
		Fence();
		~Fence();
		uint64_t GetValue() const {
			return m_FenceValue;
		}
		void IncreaseValue() {
			m_FenceValue++;
		}
		void WaitForFenceValue(uint64_t value, std::chrono::milliseconds duration = std::chrono::milliseconds::max());

		ID3D12Fence* Get() const {
			return m_D3D_Fence.Get();
		}
	};
}