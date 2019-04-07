#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"RootSigature.h"
#include"PSO.h"
#include"DescriptorAllocation.h"
namespace Dx12
{
	struct alignas(16) GenerateMipsCB
	{
		uint32_t SrcMipLevel;	// Texture level of source mip
		uint32_t NumMipLevels;	// Number of OutMips to write: [1-4]
		uint32_t SrcDimension;  // Width and height of the source texture are even or odd.
		uint32_t Padding;       // Pad to 16 byte alignment.
		DirectX::XMFLOAT2 TexelSize;	// 1.0 / OutMip1.Dimensions
	};
	namespace GenerateMips
	{
		enum
		{
			GenerateMipsCB,
			SrcMip,
			OutMip,
			NumRootParameters
		};
	}

	class GenerateMipMapsCS
	{
	private:
		RootSignature m_RootSig;
		ComputePSO    m_PSO;
		DescriptorAllocation m_UAV;
	public:
		GenerateMipMapsCS();

		const ComputePSO& GetPSO() const{
			return m_PSO;
		}
		const RootSignature& GetRootSig() const{
			return m_RootSig;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetUAV(uint32_t offset) const {
			return m_UAV.GetDescriptorHandle(offset);
		}
	};
}