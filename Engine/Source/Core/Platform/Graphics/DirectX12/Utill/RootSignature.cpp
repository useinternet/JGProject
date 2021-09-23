#include "pch.h"
#include "RootSignature.h"
#include "Platform/Graphics/DirectX12/DirectX12API.h"




namespace JG
{
	static Dictionary<JG::u64, ComPtr<ID3D12RootSignature>> gRootSigCahce;


	void RootSignature::InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, u32 numDescriptor, u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility)
	{
		u32 rootParam = (u32)mRootSigInitType.size();
		mDescriptorTableInfoByRootParam[rootParam] = DescriptorTableInfo(type, numDescriptor);

		auto range = CreateUniquePtr<CD3DX12_DESCRIPTOR_RANGE>();
		range->Init(type, numDescriptor, register_num, register_space);

		CD3DX12_ROOT_PARAMETER param;
		param.InitAsDescriptorTable(1, range.get(), visibility);


		mRootParams.push_back(param);
		mDescriptorRanges.push_back(move(range));

		mRootSigInitType.push_back(__DescriptorTable__);
	}
	void RootSignature::InitAsSRV(u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility)
	{
		CD3DX12_ROOT_PARAMETER param;

		param.InitAsShaderResourceView(register_num, register_space, visibility);
		mRootParams.push_back(param);



		mRootSigInitType.push_back(__ShaderResourceView__);
	}
	void RootSignature::InitAsUAV(u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility)
	{
		CD3DX12_ROOT_PARAMETER param;

		param.InitAsUnorderedAccessView(register_num, register_space, visibility);
		mRootParams.push_back(param);

		mRootSigInitType.push_back(__UnorderedAccessView__);
	}
	void RootSignature::InitAsCBV(u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility)
	{
		CD3DX12_ROOT_PARAMETER param;

		param.InitAsConstantBufferView(register_num, register_space, visibility);
		mRootParams.push_back(param);

		mRootSigInitType.push_back(__ConstantBufferView__);
	}
	void RootSignature::InitAsConstant(u32 btSize, u32 register_num, u32 register_space, D3D12_SHADER_VISIBILITY visibility)
	{
		CD3DX12_ROOT_PARAMETER param;

		param.InitAsConstants(btSize / 4, register_num, register_space, visibility);
		mRootParams.push_back(param);

		mRootSigInitType.push_back(__Constant__);
	}
	void RootSignature::AddStaticSamplerState(const D3D12_STATIC_SAMPLER_DESC& desc)
	{
		mSamplerState.push_back(desc);
	}
	RootSignature::DescriptorTableInfo RootSignature::GetDescriptorTableRangeType(u32 rootparam) const
	{
		JGASSERT_IF(mDescriptorTableInfoByRootParam.find(rootparam) != mDescriptorTableInfoByRootParam.end(),
			"GetDescriptorTableInfo : nonexist DescriptorTable in rootparam");

		return mDescriptorTableInfoByRootParam.at(rootparam);
	}
	void RootSignature::Reset()
	{
		mD3DRootSig.Reset();
		mRootParams.clear();
		mSamplerState.clear();
		mRootSigInitType.clear();
		mDescriptorTableInfoByRootParam.clear();
		mDescriptorRanges.clear();
	}
	bool RootSignature::Finalize()
	{
		if (mD3DRootSig != nullptr)
		{
			return false;
		}


		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc((u32)mRootParams.size(),
			mRootParams.data(), (u32)mSamplerState.size(), mSamplerState.data(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		u64 HashCode = HashState(&rootSigDesc.Flags);

		HashCode = HashState(rootSigDesc.pStaticSamplers, mSamplerState.size(), HashCode);



		for (u32 param = 0; param < (u32)mRootParams.size(); ++param)
		{
			const D3D12_ROOT_PARAMETER& RootParam = rootSigDesc.pParameters[param];

			if (RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			{
				HashCode = HashState(RootParam.DescriptorTable.pDescriptorRanges,
					RootParam.DescriptorTable.NumDescriptorRanges, HashCode);

			}
			else
				HashCode = HashState(&RootParam, 1, HashCode);
		}

		ID3D12RootSignature** RSRef = nullptr;
		bool firstCompile = false;
		{
			static std::mutex s_HashMapMutex;
			std::lock_guard<std::mutex> CS(s_HashMapMutex);
			auto iter = gRootSigCahce.find(HashCode);

			// Reserve space so the next inquiry will find that someone got here first.
			if (iter == gRootSigCahce.end())
			{
				RSRef = gRootSigCahce[HashCode].GetAddressOf();
				firstCompile = true;
			}
			else
				RSRef = iter->second.GetAddressOf();
		}

		if (firstCompile)
		{
			mD3DRootSig = CreateD3DRootSignature(DirectX12API::GetD3DDevice(), &rootSigDesc);
			if (mD3DRootSig == nullptr) assert("failed CreateRootSig");
			gRootSigCahce[HashCode] = mD3DRootSig.Get();
		}
		else
		{
			while (*RSRef == nullptr)
				std::this_thread::yield();
			mD3DRootSig = *RSRef;
		}

		mDescriptorRanges.clear();
		mRootParams.clear();
		return true;
	}
	void RootSignature::ClearCache()
	{
		gRootSigCahce.clear();
	}
}