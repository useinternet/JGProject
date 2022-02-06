#pragma once

#include "JGCore.h"
#include "DirectX12Helper.h"



namespace JG
{
	class RootSignature
	{
		friend class DynamicDescriptorAllocator;
	public:
		enum
		{
			__DescriptorTable__,
			__ShaderResourceView__,
			__UnorderedAccessView__,
			__ConstantBufferView__,
			__Constant__
		};
		struct DescriptorTableInfo
		{
			D3D12_DESCRIPTOR_RANGE_TYPE Type;
			u32 NumDescirptor;


			DescriptorTableInfo() = default;
			DescriptorTableInfo(D3D12_DESCRIPTOR_RANGE_TYPE type, u32 numDescriptor) :
				Type(type), NumDescirptor(numDescriptor) {}
		};
	private:
		ComPtr<ID3D12RootSignature>		 mD3DRootSig;
		List<CD3DX12_ROOT_PARAMETER>     mRootParams;
		List<D3D12_STATIC_SAMPLER_DESC>  mSamplerState;


		List<i32> mRootSigInitType;
		SortedDictionary<u32, DescriptorTableInfo> mDescriptorTableInfoByRootParam;
		List<UniquePtr<D3D12_DESCRIPTOR_RANGE>> mDescriptorRanges;

	public: 
		void InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, u32 numDescriptor, u32 register_num,
			u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsSRV(u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsUAV(u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsCBV(u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void InitAsConstant(u32 btSize, u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
		void AddStaticSamplerState(const D3D12_STATIC_SAMPLER_DESC& desc);
		DescriptorTableInfo GetDescriptorTableRangeType(u32 rootparam) const;
		void Reset();
		bool Finalize(D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	public:
		ID3D12RootSignature* Get() const {
			return mD3DRootSig.Get();
		}
		ID3D12RootSignature** GetAddressOf() {
			return mD3DRootSig.GetAddressOf();
		}
	private:
		friend class DirectX12API;
		static void ClearCache();
	};
}



