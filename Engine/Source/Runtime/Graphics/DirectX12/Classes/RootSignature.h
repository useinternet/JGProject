#pragma once
#include "Core.h"
#include "DirectX12Helper.h"

class PRootSignature : public IMemoryObject
{
	friend class PDynamicDescriptionAllocator;
public:
	enum
	{
		__DescriptorTable__,
		__ShaderResourceView__,
		__UnorderedAccessView__,
		__ConstantBufferView__,
		__Constant__
	};

	struct HDescriptorTableInfo
	{
		D3D12_DESCRIPTOR_RANGE_TYPE Type;
		uint32 NumDescirptor;

		HDescriptorTableInfo() = default;
		HDescriptorTableInfo(D3D12_DESCRIPTOR_RANGE_TYPE type, uint32 numDescriptor) :
			Type(type), NumDescirptor(numDescriptor) {}
	};
private:
	HDX12ComPtr<HDX12RootSignature>	  _dx12RootSig;
	HList<CD3DX12_ROOT_PARAMETER>     _rootParams;
	HList<D3D12_STATIC_SAMPLER_DESC>  _samplerState;


	HList<int32> _rootSigInitType;
	HMap<uint32, HDescriptorTableInfo> _descriptorTableInfoByRootParam;
	HList<HSTLUniquePtr<D3D12_DESCRIPTOR_RANGE>> _descriptorRanges;

public:
	virtual ~PRootSignature() = default;

	void InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, uint32 numDescriptor, uint32 register_num,
		uint32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitAsSRV(uint32 register_num, uint32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitAsUAV(uint32 register_num, uint32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitAsCBV(uint32 register_num, uint32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	void InitAsConstant(uint32 btSize, uint32 register_num, uint32 register_space, D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);
	void AddStaticSamplerState(const D3D12_STATIC_SAMPLER_DESC& desc);
	HDescriptorTableInfo GetDescriptorTableRangeType(uint32 rootparam) const;
	void Reset();
	bool Finalize(D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

public:
	ID3D12RootSignature* Get() const {
		return _dx12RootSig.Get();
	}
	ID3D12RootSignature** GetAddressOf() {
		return _dx12RootSig.GetAddressOf();
	}
};