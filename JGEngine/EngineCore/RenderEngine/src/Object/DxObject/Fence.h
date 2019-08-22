#pragma once
#include "DxObject.h"

namespace RE
{
	class Fence : public DxObject
	{
	private:
		ComPtr<ID3D12Fence>  m_D3D_Fence;
		std::mutex m_EventMutex;
		std::atomic_uint64_t m_FenceValue;
		HANDLE   m_FenceEvent;
	public:
		Fence();
		~Fence();
		// Fence 값 가져오기
		uint64_t GetValue() const {
			return m_FenceValue;
		}
		// Fence 값 증가
		void IncreaseValue() {
			m_FenceValue.fetch_add(1);
		}
		// Fence 값까지 동기화 
		void WaitForFenceValue(uint64_t value, std::chrono::milliseconds duration = std::chrono::milliseconds::max());

		// ID3D12Fence* 가져오기
		ID3D12Fence* Get() const {
			return m_D3D_Fence.Get();
		}
	};
}