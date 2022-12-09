#include "PCH/PCH.h"
#include "RootSignature.h"
#include "DirectX12/DirectX12API.h"

void PRootSignature::InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, uint32 numDescriptor, uint32 register_num, uint32 register_space, D3D12_SHADER_VISIBILITY visibility)
{
	uint32 rootParam = (uint32)_rootSigInitType.size();
	_descriptorTableInfoByRootParam[rootParam] = HDescriptorTableInfo(type, numDescriptor);

	HSTLUniquePtr<CD3DX12_DESCRIPTOR_RANGE> range = std::make_unique<CD3DX12_DESCRIPTOR_RANGE>();
	range->Init(type, numDescriptor, register_num, register_space);

	CD3DX12_ROOT_PARAMETER param;
	param.InitAsDescriptorTable(1, range.get(), visibility);

	_rootParams.push_back(param);
	_descriptorRanges.push_back(move(range));

	_rootSigInitType.push_back(__DescriptorTable__);
}

void PRootSignature::InitAsSRV(uint32 register_num, uint32 register_space, D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param;

	param.InitAsShaderResourceView(register_num, register_space, visibility);
	_rootParams.push_back(param);

	_rootSigInitType.push_back(__ShaderResourceView__);
}

void PRootSignature::InitAsUAV(uint32 register_num, uint32 register_space, D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param;

	param.InitAsUnorderedAccessView(register_num, register_space, visibility);
	_rootParams.push_back(param);

	_rootSigInitType.push_back(__UnorderedAccessView__);
}

void PRootSignature::InitAsCBV(uint32 register_num, uint32 register_space, D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param;

	param.InitAsConstantBufferView(register_num, register_space, visibility);
	_rootParams.push_back(param);

	_rootSigInitType.push_back(__ConstantBufferView__);
}

void PRootSignature::InitAsConstant(uint32 btSize, uint32 register_num, uint32 register_space, D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param;

	param.InitAsConstants(btSize / 4, register_num, register_space, visibility);
	_rootParams.push_back(param);

	_rootSigInitType.push_back(__Constant__);
}

void PRootSignature::AddStaticSamplerState(const D3D12_STATIC_SAMPLER_DESC& desc)
{
	_samplerState.push_back(desc);
}

PRootSignature::HDescriptorTableInfo PRootSignature::GetDescriptorTableRangeType(uint32 rootparam) const
{
	JG_ASSERT_IF(_descriptorTableInfoByRootParam.find(rootparam) != _descriptorTableInfoByRootParam.end(),
		"GetDescriptorTableInfo : nonexist DescriptorTable in rootparam");

	return _descriptorTableInfoByRootParam.at(rootparam);
}
void PRootSignature::Reset()
{
	_dx12RootSig.Reset();
	_rootParams.clear();
	_samplerState.clear();
	_rootSigInitType.clear();
	_descriptorTableInfoByRootParam.clear();
	_descriptorRanges.clear();
}
bool PRootSignature::Finalize(D3D12_ROOT_SIGNATURE_FLAGS flags)
{
	if (_dx12RootSig != nullptr)
	{
		return false;
	}

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc((uint32)_rootParams.size(),
		_rootParams.data(), (uint32)_samplerState.size(), _samplerState.data(),
		flags);

	uint64 HashCode = HHash::HashState(&rootSigDesc.Flags);
	HashCode = HHash::HashState(rootSigDesc.pStaticSamplers, _samplerState.size(), HashCode);

	for (uint32 param = 0; param < (uint32)_rootParams.size(); ++param)
	{
		const D3D12_ROOT_PARAMETER& RootParam = rootSigDesc.pParameters[param];

		if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			HashCode = HHash::HashState(RootParam.DescriptorTable.pDescriptorRanges,
				RootParam.DescriptorTable.NumDescriptorRanges, HashCode);

		}
		else
			HashCode = HHash::HashState(&RootParam, 1, HashCode);
	}

	HHashMap<uint64, HDX12ComPtr<HDX12RootSignature>>& globalRootSigCache = HDirectXAPI::GetRootSignatureCacheRef();
	ID3D12RootSignature** RSRef = nullptr;
	bool bFirstCompile = false;
	{
		static HMutex sHashMapMutex;
		HLockGuard<HMutex> CS(sHashMapMutex);

		auto iter = globalRootSigCache.find(HashCode);
		// Reserve space so the next inquiry will find that someone got here first.
		if (iter == globalRootSigCache.end())
		{
			RSRef = globalRootSigCache[HashCode].GetAddressOf();
			bFirstCompile = true;
		}
		else
			RSRef = iter->second.GetAddressOf();
	}

	if (bFirstCompile)
	{
		_dx12RootSig = HDirectX12Helper::CreateD3DRootSignature(HDirectXAPI::GetDevice(), &rootSigDesc);
		if (_dx12RootSig == nullptr) JG_ASSERT("failed CreateRootSig");
		globalRootSigCache[HashCode] = _dx12RootSig.Get();
	}
	else
	{
		while (*RSRef == nullptr)
			std::this_thread::yield();
		_dx12RootSig = *RSRef;
	}

	_descriptorRanges.clear();
	_rootParams.clear();

	return true;
}