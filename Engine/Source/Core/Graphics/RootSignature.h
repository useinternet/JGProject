#pragma once


#include "JGCore.h"
#include "GraphicsDefine.h"







namespace JG
{
	enum class EDescriptorTableRangeType
	{
		NONE,
		SRV,
		CBV,
		UAV
	};

	enum class ESamplerFilter
	{
		Point,
		Linear,
		Anisotropic,
	};
	enum class ETextureAddressMode
	{
		Wrap,
		Mirror,
		Clamp,
		Border,
		MirrorOnce,
	};



	class IRootSignature
	{
	public:
		virtual ~IRootSignature() = default;
	};
	class IRootSignatureCreater
	{
	public:
		virtual ~IRootSignatureCreater() = default;
	public:
		virtual void AddDescriptorTable(u32 rootParam, EDescriptorTableRangeType rangeType, u32 numDescriptor, u32 numRegister, u32 numRegisterSpace) = 0;
		virtual void AddSRV(u32 rootParam, u32 numRegister, u32 numRegisterSpace) = 0;
		virtual void AddCBV(u32 rootParam, u32 numRegister, u32 numRegisterSpace) = 0;
		virtual void AddConstant(u32 rootParam, u32 btSize, u32 numRegister, u32 numRegisterSpace) = 0;
		virtual void AddUAV(u32 rootParam, u32 numRegister, u32 numRegisterSpace) = 0;
		virtual void AddSampler(u32 numRegister, ESamplerFilter filter, ETextureAddressMode addressMode) = 0;
		virtual void Reset() = 0;
		virtual SharedPtr<IRootSignature> Generate(bool isLocal = false) = 0;
	public:
		static SharedPtr<IRootSignatureCreater> Create();
	};
}