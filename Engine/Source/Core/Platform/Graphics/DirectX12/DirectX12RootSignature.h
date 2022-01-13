#pragma once


#include "Graphics/RootSignature.h"
#include "Utill/DirectX12Helper.h"

namespace JG
{
	class RootSignature;
	class DirectX12RootSignature : public IRootSignature
	{
		friend class DirectX12RootSignatureCreater;
		SharedPtr<RootSignature> mRootSig;
		Dictionary<u32, EDescriptorTableRangeType> mDescriptorTableRangeTypeDic;
	public:
		EDescriptorTableRangeType GetDescriptorRangeType(u32 rootParam) const;
		SharedPtr<RootSignature> Get() const {
			return mRootSig;
		}
	};
	class DirectX12RootSignatureCreater : public IRootSignatureCreater
	{
		enum EType
		{
			SRV,
			UAV,
			CBV,
			DT_SRV,
			DT_UAV,
			DT_CBV
		};
		struct Data
		{
			EType Type;
			u32 numDescriptor	 = 0;
			u32 numRegister		 = 0;
			u32 numRegisterSpace = 0;
		};
		struct SamplerData
		{
			u32 numRegister	= 0;
			ESamplerFilter Filter;
			ETextureAddressMode AddressMode;
			Color BorderColor;
		};
	private:
		SortedDictionary<u32, Data> mRootSignatureDataDic;
		SortedDictionary<u32, SamplerData> mRootSignatureSamplerDataDic;
	public:
		virtual void AddDescriptorTable(u32 rootParam, EDescriptorTableRangeType rangeType, u32 numDescriptor, u32 numRegister, u32 numRegisterSpace) override;
		virtual void AddSRV(u32 rootParam, u32 numRegister, u32 numRegisterSpace) override;
		virtual void AddCBV(u32 rootParam, u32 numRegister, u32 numRegisterSpace) override;
		virtual void AddUAV(u32 rootParam, u32 numRegister, u32 numRegisterSpace) override;
		virtual void AddSampler(u32 numRegister, ESamplerFilter filter, ETextureAddressMode addressMode) override;
		virtual void Reset() override;
		virtual SharedPtr<IRootSignature> Generate() override;
	private:
		D3D12_FILTER ToD3D(ESamplerFilter filter);
		D3D12_TEXTURE_ADDRESS_MODE ToD3D(ETextureAddressMode addressMode);
	};

}