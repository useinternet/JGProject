#pragma once
#include"Buffer.h"
#include"DescriptorAllocation.h"

namespace Dx12
{
	class ConstantBuffer : public Buffer
	{
	private:
		DescriptorAllocation m_ConstantBufferView;
		uint32_t m_SizeInBytes;
	public:
		ConstantBuffer(const std::string& name = "ConstantBuffer");
		virtual ~ConstantBuffer();

		virtual void CreateViews(uint32_t numElements, uint32_t elementSize) override;

	public:
		uint32_t GetSizeInBytes() const {
			return m_SizeInBytes;
		}


		D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferView() const
		{
			return m_ConstantBufferView.GetDescriptorHandle();
		}
	};
}