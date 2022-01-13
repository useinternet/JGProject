#include "pch.h"
#include "DirectX12RootSignature.h"
#include "Utill/RootSignature.h"
namespace JG
{
	EDescriptorTableRangeType DirectX12RootSignature::GetDescriptorRangeType(u32 rootParam) const
	{
		if (mDescriptorTableRangeTypeDic.find(rootParam) == mDescriptorTableRangeTypeDic.end())
		{
			return EDescriptorTableRangeType::NONE;
		}
		return mDescriptorTableRangeTypeDic.at(rootParam);
	}

	void DirectX12RootSignatureCreater::AddDescriptorTable(u32 rootParam, EDescriptorTableRangeType rangeType, u32 numDescriptor, u32 numRegister, u32 numRegisterSpace)
	{
		Data data;
		data.numDescriptor    = numDescriptor;
		data.numRegister	  = numRegister;
		data.numRegisterSpace = numRegisterSpace;

		switch (rangeType)
		{
		case EDescriptorTableRangeType::CBV:
			data.Type = DT_CBV;
			break;
		case EDescriptorTableRangeType::SRV:
			data.Type = DT_UAV;
			break;
		case EDescriptorTableRangeType::UAV:
			data.Type = DT_UAV;
			break;
		}

		mRootSignatureDataDic[rootParam] = data;
	}

	void DirectX12RootSignatureCreater::AddSRV(u32 rootParam, u32 numRegister, u32 numRegisterSpace)
	{
		Data data;
		data.Type = SRV;
		data.numRegister	  = numRegister;
		data.numRegisterSpace = numRegisterSpace;
		mRootSignatureDataDic[rootParam] = data;
	}

	void DirectX12RootSignatureCreater::AddCBV(u32 rootParam, u32 numRegister, u32 numRegisterSpace)
	{
		Data data;
		data.Type = CBV;
		data.numRegister = numRegister;
		data.numRegisterSpace = numRegisterSpace;
		mRootSignatureDataDic[rootParam] = data;
	}

	void DirectX12RootSignatureCreater::AddUAV(u32 rootParam, u32 numRegister, u32 numRegisterSpace)
	{
		Data data;
		data.Type = UAV;
		data.numRegister = numRegister;
		data.numRegisterSpace = numRegisterSpace;
		mRootSignatureDataDic[rootParam] = data;
	}

	void DirectX12RootSignatureCreater::AddSampler(u32 numRegister, ESamplerFilter filter, ETextureAddressMode addressMode)
	{
		SamplerData data;
		data.numRegister = numRegister;
		data.Filter = filter;
		data.AddressMode = addressMode;

		mRootSignatureSamplerDataDic[numRegister] = data;
	}

	void DirectX12RootSignatureCreater::Reset()
	{
		mRootSignatureDataDic.clear();
		mRootSignatureSamplerDataDic.clear();
	}

	SharedPtr<IRootSignature> DirectX12RootSignatureCreater::Generate()
	{

		if (mRootSignatureDataDic.empty())
		{
			return nullptr;
		}
		SharedPtr<DirectX12RootSignature> result = CreateSharedPtr< DirectX12RootSignature>();
		auto rootSig = CreateSharedPtr<RootSignature>();
		for (auto& _pair : mRootSignatureDataDic)
		{
			u32 rootParam = _pair.first;
			Data data = _pair.second;


			switch (data.Type)
			{
			case SRV:
				rootSig->InitAsSRV(data.numRegister, data.numRegisterSpace);
				result->mDescriptorTableRangeTypeDic[rootParam] = EDescriptorTableRangeType::NONE;
				break;
			case UAV:
				rootSig->InitAsUAV(data.numRegister, data.numRegisterSpace);
				result->mDescriptorTableRangeTypeDic[rootParam] = EDescriptorTableRangeType::NONE;
				break;
			case CBV:
				rootSig->InitAsCBV(data.numRegister, data.numRegisterSpace);
				result->mDescriptorTableRangeTypeDic[rootParam] = EDescriptorTableRangeType::NONE;
				break;
			case DT_SRV:
				rootSig->InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, data.numDescriptor, data.numRegister, data.numRegisterSpace);
				result->mDescriptorTableRangeTypeDic[rootParam] = EDescriptorTableRangeType::SRV;
				break;
			case DT_UAV:
				rootSig->InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, data.numDescriptor, data.numRegister, data.numRegisterSpace);
				result->mDescriptorTableRangeTypeDic[rootParam] = EDescriptorTableRangeType::UAV;
				break;
			case DT_CBV:
				rootSig->InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, data.numDescriptor, data.numRegister, data.numRegisterSpace);
				result->mDescriptorTableRangeTypeDic[rootParam] = EDescriptorTableRangeType::CBV;
				break;
			}
		}


		for (auto& _pair : mRootSignatureSamplerDataDic)
		{
			u32 numRegister = _pair.first;
			SamplerData data = _pair.second;


			const CD3DX12_STATIC_SAMPLER_DESC sampler(
				numRegister, // shaderRegister
				ToD3D(data.Filter), // filter
				ToD3D(data.AddressMode),  // addressU
				ToD3D(data.AddressMode),  // addressV
				ToD3D(data.AddressMode),
				0.0f, 16, D3D12_COMPARISON_FUNC_EQUAL,
				D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK); // addressW
			rootSig->AddStaticSamplerState(sampler);
		}

		if (rootSig->Finalize())
		{
			result->mRootSig = rootSig;
			return result;

		}
		return nullptr;
	}

	D3D12_FILTER DirectX12RootSignatureCreater::ToD3D(ESamplerFilter filter)
	{
		switch (filter)
		{
		case ESamplerFilter::Point:		 return D3D12_FILTER_MIN_MAG_MIP_POINT;
		case ESamplerFilter::Linear:	 return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		case ESamplerFilter::Anisotropic:return D3D12_FILTER_ANISOTROPIC;
		}
		return D3D12_FILTER_MIN_MAG_MIP_POINT;
	}

	D3D12_TEXTURE_ADDRESS_MODE DirectX12RootSignatureCreater::ToD3D(ETextureAddressMode addressMode)
	{
		switch (addressMode)
		{
		case ETextureAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case ETextureAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case ETextureAddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		case ETextureAddressMode::Border: return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		case ETextureAddressMode::MirrorOnce: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE;
		}
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}





}