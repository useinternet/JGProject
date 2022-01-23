#include "pch.h"
#include "PipelineState.h"
#include "RootSignature.h"

#include "Platform/Graphics/DirectX12/DirectX12API.h"
#include "Platform/Graphics/DirectX12/DirectX12Shader.h"
namespace JG
{
	Dictionary<u64, ComPtr<ID3D12PipelineState>> gGraphicsPSOCache;
	Dictionary<u64, ComPtr<ID3D12PipelineState>> gComputePSOCache;
	


	DXGI_FORMAT ConvertShaderDataType(EShaderDataType type)
	{
		switch (type)
		{
		case EShaderDataType::_float: return DXGI_FORMAT_R32_FLOAT;
		case EShaderDataType::_float2: return DXGI_FORMAT_R32G32_FLOAT;
		case EShaderDataType::_float3: return DXGI_FORMAT_R32G32B32_FLOAT;
		case EShaderDataType::_float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;

		case EShaderDataType::_int: return  DXGI_FORMAT_R32_SINT;
		case EShaderDataType::_int2: return DXGI_FORMAT_R32G32_SINT;
		case EShaderDataType::_int3: return DXGI_FORMAT_R32G32B32_SINT;
		case EShaderDataType::_int4: return DXGI_FORMAT_R32G32B32A32_SINT;

		case EShaderDataType::_uint: return DXGI_FORMAT_R32_UINT;
		case EShaderDataType::_uint2: return DXGI_FORMAT_R32G32_UINT;
		case EShaderDataType::_uint3: return DXGI_FORMAT_R32G32B32_UINT;
		case EShaderDataType::_uint4: return DXGI_FORMAT_R32G32B32A32_UINT;

		default:
			JG_CORE_ERROR("ConvertShaderDataType :: not supported ShaderDataType");
			return DXGI_FORMAT_UNKNOWN;
		}
		

	}

	void PipelineState::ClearCache()
	{
		gGraphicsPSOCache.clear();
		gComputePSOCache.clear();
	}

	GraphicsPipelineState::GraphicsPipelineState()
	{
		mDesc.SampleMask        = UINT_MAX;
		mDesc.RasterizerState   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		mDesc.BlendState        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		mDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		mDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		mDesc.SampleDesc.Count   = 1;
		mDesc.SampleDesc.Quality = 0;
	}

	void GraphicsPipelineState::BindRootSignature(const RootSignature& rootSig)
	{
		mIsDirty = true;
		mDesc.pRootSignature = rootSig.Get();
	}

	void GraphicsPipelineState::BindRenderTarget(const List<DXGI_FORMAT>& rtFormats, DXGI_FORMAT dvFormat)
	{
		mIsDirty = true;

		u64 cnt = rtFormats.size();
		if (cnt >= MAX_RENDERTARGET)
		{
			JG_CORE_ERROR("RenderTarget Num can not exceed 8");
			cnt = MAX_RENDERTARGET;
		}


		mDesc.NumRenderTargets = (u32)cnt;
		for (u64 i = 0; i < MAX_RENDERTARGET; ++i)
		{
			if (i < cnt)
			{
				mDesc.RTVFormats[i] = rtFormats[i];
			}
			else
			{
				mDesc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
			}
		}
		mDesc.DSVFormat = dvFormat;
	}

	void GraphicsPipelineState::BindInputLayout(const InputLayout& inputLayout)
	{
		mIsDirty = true;
		mD3DInputLayoutDescs.clear();
		u32 offset = 0;
		inputLayout.ForEach([&](const InputElement& element)
		{
			D3D12_INPUT_ELEMENT_DESC Desc = {};
			Desc.SemanticIndex = element.SementicSlot;
			Desc.SemanticName = element.SementicName;
			Desc.Format = ConvertShaderDataType(element.Type);
			Desc.InputSlot = 0;
			Desc.AlignedByteOffset = offset;
			Desc.InstanceDataStepRate = 0;
			Desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			mD3DInputLayoutDescs.push_back(Desc);
			offset += (u32)GetShaderDataTypeSize(element.Type);
		});

		mDesc.InputLayout.NumElements        = (u32)mD3DInputLayoutDescs.size();
		mDesc.InputLayout.pInputElementDescs = mD3DInputLayoutDescs.data();

	}
	void GraphicsPipelineState::BindShader(const DirectX12GraphicsShader& shader)
	{
		mIsDirty = true;
		auto flags = shader.GetFlags();


		if (shader.GetVSData() != nullptr)
		{
			mDesc.VS = {
				reinterpret_cast<byte*>(shader.GetVSData()->GetBufferPointer()),
				shader.GetVSData()->GetBufferSize()
			};
		}

		if (shader.GetDSData() != nullptr)
		{
			mDesc.DS = {
				reinterpret_cast<byte*>(shader.GetDSData()->GetBufferPointer()),
				shader.GetDSData()->GetBufferSize()
			};
		}

		if (shader.GetHSData() != nullptr)
		{
			mDesc.HS = {
				reinterpret_cast<byte*>(shader.GetHSData()->GetBufferPointer()),
				shader.GetHSData()->GetBufferSize()
			};
		}

		if (shader.GetGSData() != nullptr)
		{
			mDesc.GS = {
				reinterpret_cast<byte*>(shader.GetGSData()->GetBufferPointer()),
				shader.GetGSData()->GetBufferSize()
			};
		}

		if (shader.GetPSData() != nullptr)
		{
			mDesc.PS = {
				reinterpret_cast<byte*>(shader.GetPSData()->GetBufferPointer()),
				shader.GetPSData()->GetBufferSize()
			};
		}
	}

	void GraphicsPipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
	{
		mIsDirty = true;
		mDesc.PrimitiveTopologyType = type;
	}

	void GraphicsPipelineState::SetSampleMask(uint32_t sampleMask)
	{
		mIsDirty = true;
		mDesc.SampleMask = sampleMask;
	}

	void GraphicsPipelineState::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
	{
		mIsDirty = true;
		mDesc.RasterizerState = desc;
	}

	void GraphicsPipelineState::SetBlendState(const D3D12_BLEND_DESC& desc)
	{
		mIsDirty = true;
		mDesc.BlendState = desc;
	}

	void GraphicsPipelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
	{
		mIsDirty = true;
		mDesc.DepthStencilState = desc;
	}

	bool GraphicsPipelineState::Finalize()
	{
		if (mIsDirty == false && mD3DPSO != nullptr)
		{
			return true;
		}


		mDesc.InputLayout.pInputElementDescs = nullptr;
		u64 hash = HashState(&mDesc);

		mDesc.InputLayout.pInputElementDescs = mD3DInputLayoutDescs.data();

		ID3D12PipelineState** PSORef = nullptr;
		bool firstCompile = false;


		{
			static std::mutex s_HashMapMutex;
			std::lock_guard<std::mutex> lock(s_HashMapMutex);
			auto iter = gGraphicsPSOCache.find(hash);

			if (iter == gGraphicsPSOCache.end())
			{
				firstCompile = true;
				PSORef = gGraphicsPSOCache[hash].GetAddressOf();
			}
			else
				PSORef = iter->second.GetAddressOf();
		}


		if (firstCompile)
		{
			HRESULT hr = DirectX12API::GetD3DDevice()->CreateGraphicsPipelineState(&mDesc, IID_PPV_ARGS(mD3DPSO.GetAddressOf()));
			if (FAILED(hr))
			{
				return false;
			} 
			gGraphicsPSOCache[hash] = mD3DPSO;
		}
		else
		{
			while (*PSORef == nullptr)
				std::this_thread::yield();
			mD3DPSO = *PSORef;
		}

		mIsDirty = false;
		return true;
	}

	const D3D12_BLEND_DESC& GraphicsPipelineState::GetBlendDesc() const
	{
		return mDesc.BlendState;
	}



	ComputePipelineState::ComputePipelineState()
	{
	}

	void ComputePipelineState::BindRootSignature(const RootSignature& rootSig)
	{
		mIsDirty = mDesc.pRootSignature != rootSig.Get();
		mDesc.pRootSignature = rootSig.Get();
	}

	void ComputePipelineState::BindShader(const DirectX12ComputeShader& shader)
	{
		mIsDirty = true;
		if (shader.GetCSData() != nullptr)
		{
			mDesc.CS = {
				reinterpret_cast<byte*>(shader.GetCSData()->GetBufferPointer()),
				shader.GetCSData()->GetBufferSize()
			};
		}

	}

	bool ComputePipelineState::Finalize()
	{
		if (mIsDirty == false && mD3DPSO != nullptr)
		{
			return true;
		}


		u64 hash = HashState(&mDesc);

		ID3D12PipelineState** PSORef = nullptr;
		bool firstCompile = false;
		{
			static std::mutex s_HashMapMutex;
			std::lock_guard<std::mutex> CS(s_HashMapMutex);
			auto iter = gComputePSOCache.find(hash);

			if (iter == gComputePSOCache.end())
			{
				firstCompile = true;
				PSORef = gComputePSOCache[hash].GetAddressOf();
			}
			else
				PSORef = iter->second.GetAddressOf();
		}

		if (firstCompile)
		{
			HRESULT hr = DirectX12API::GetD3DDevice()->CreateComputePipelineState(&mDesc, IID_PPV_ARGS(&mD3DPSO));
			if (FAILED(hr))
			{
				return false;
				assert("failed Create Compute PSO");
			}
			gComputePSOCache[hash] = mD3DPSO.Get();
		}
		else
		{
			while (*PSORef == nullptr)
				std::this_thread::yield();
			mD3DPSO = *PSORef;
		}
		mIsDirty = false;
		return true;
	}

}