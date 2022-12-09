#include "PCH/PCH.h"
#include "PipelineState.h"
#include "JGGraphicsHelper.h"
#include "DirectX12/DirectX12API.h"
#include "DX12Shader.h"
#include "RootSignature.h"

PGraphicsPipelineState::PGraphicsPipelineState()
{
	_desc.SampleMask			= UINT_MAX;
	_desc.RasterizerState		= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	_desc.BlendState			= CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	_desc.DepthStencilState     = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	_desc.SampleDesc.Count      = 1;
	_desc.SampleDesc.Quality    = 0;
}

void PGraphicsPipelineState::BindRootSignature(const PRootSignature& rootSig)
{
	_bDirty = true;
	_desc.pRootSignature = rootSig.Get();
}

void PGraphicsPipelineState::BindRenderTarget(const HList<DXGI_FORMAT>& rtFormats, DXGI_FORMAT dvFormat)
{
	_bDirty = true;

	uint64 cnt = rtFormats.size();
	if (cnt >= MAX_RENDERTARGET)
	{
		JG_LOG(Graphics, ELogLevel::Error, "RenderTarget Num can not exceed 8");
		cnt = MAX_RENDERTARGET;
	}

	_desc.NumRenderTargets = (uint32)cnt;
	for (uint64 i = 0; i < MAX_RENDERTARGET; ++i)
	{
		if (i < cnt)
		{
			_desc.RTVFormats[i] = rtFormats[i];
		}
		else
		{
			_desc.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
		}
	}
	_desc.DSVFormat = dvFormat;
}

void PGraphicsPipelineState::BindInputLayout(const HInputLayout& inputLayout)
{
	_bDirty = true;
	_inputLayoutDescs.clear();
	uint32 offset = 0;
	inputLayout.ForEach([&](const HInputElement& element)
	{
		D3D12_INPUT_ELEMENT_DESC Desc = {};
		Desc.SemanticIndex = element.SementicSlot;
		Desc.SemanticName  = element.SementicName;
		Desc.Format = HDirectX12Helper::ConvertDXGIFormat(element.Type);
		Desc.InputSlot = 0;
		Desc.AlignedByteOffset = offset;
		Desc.InstanceDataStepRate = 0;
		Desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		_inputLayoutDescs.push_back(Desc);
		offset += (uint32)(HJGGraphicsHelper::GetShaderDataTypeSize(element.Type));
	});

	_desc.InputLayout.NumElements = (uint32)_inputLayoutDescs.size();
	_desc.InputLayout.pInputElementDescs = _inputLayoutDescs.data();

}
void PGraphicsPipelineState::BindShader(const PDX12GraphicsShader& shader)
{
	_bDirty = true;
	EShaderFlags flags = shader.GetFlags();


	if (shader.GetVSData() != nullptr)
	{
		_desc.VS = {
			reinterpret_cast<byte*>(shader.GetVSData()->GetBufferPointer()),
			shader.GetVSData()->GetBufferSize()
		};
	}

	if (shader.GetDSData() != nullptr)
	{
		_desc.DS = {
			reinterpret_cast<byte*>(shader.GetDSData()->GetBufferPointer()),
			shader.GetDSData()->GetBufferSize()
		};
	}

	if (shader.GetHSData() != nullptr)
	{
		_desc.HS = {
			reinterpret_cast<byte*>(shader.GetHSData()->GetBufferPointer()),
			shader.GetHSData()->GetBufferSize()
		};
	}

	if (shader.GetGSData() != nullptr)
	{
		_desc.GS = {
			reinterpret_cast<byte*>(shader.GetGSData()->GetBufferPointer()),
			shader.GetGSData()->GetBufferSize()
		};
	}

	if (shader.GetPSData() != nullptr)
	{
		_desc.PS = {
			reinterpret_cast<byte*>(shader.GetPSData()->GetBufferPointer()),
			shader.GetPSData()->GetBufferSize()
		};
	}
}

void PGraphicsPipelineState::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
{
	_bDirty = true;
	_desc.PrimitiveTopologyType = type;
}

void PGraphicsPipelineState::SetSampleMask(uint32_t sampleMask)
{
	_bDirty = true;
	_desc.SampleMask = sampleMask;
}

void PGraphicsPipelineState::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
{
	_bDirty = true;
	_desc.RasterizerState = desc;
}

void PGraphicsPipelineState::SetBlendState(const D3D12_BLEND_DESC& desc)
{
	_bDirty = true;
	_desc.BlendState = desc;
}

void PGraphicsPipelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
{
	_bDirty = true;
	_desc.DepthStencilState = desc;
}

bool PGraphicsPipelineState::Finalize()
{
	if (_bDirty == false && _dx12PSO != nullptr)
	{
		return true;
	}

	_desc.InputLayout.pInputElementDescs = nullptr;
	uint64 hash = HHash::HashState(&_desc);

	_desc.InputLayout.pInputElementDescs = _inputLayoutDescs.data();

	HDX12Pipeline** PSORef = nullptr;
	bool bFirstCompile = false;

	{
		static HMutex s_HashMapMutex;
		HLockGuard<HMutex> lock(s_HashMapMutex);
		auto iter = HDirectXAPI::GetGraphicsPSOCacheRef().find(hash);

		if (iter == HDirectXAPI::GetGraphicsPSOCache().end())
		{
			bFirstCompile = true;
			PSORef = HDirectXAPI::GetGraphicsPSOCacheRef()[hash].GetAddressOf();
		}
		else
			PSORef = iter->second.GetAddressOf();
	}


	if (bFirstCompile)
	{
		HRESULT hr = HDirectXAPI::GetDevice()->CreateGraphicsPipelineState(&_desc, IID_PPV_ARGS(_dx12PSO.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}
		HDirectXAPI::GetGraphicsPSOCacheRef()[hash] = _dx12PSO;
	}
	else
	{
		while (*PSORef == nullptr)
			std::this_thread::yield();
		_dx12PSO = *PSORef;
	}

	_bDirty = false;
	return true;
}

const D3D12_BLEND_DESC& PGraphicsPipelineState::GetBlendDesc() const
{
	return _desc.BlendState;
}



PComputePipelineState::PComputePipelineState()
{
}

void PComputePipelineState::BindRootSignature(const PRootSignature& rootSig)
{
	if (_desc.pRootSignature != rootSig.Get())
	{
		_bDirty = true;
	}

	_desc.pRootSignature = rootSig.Get();
}

void PComputePipelineState::BindShader(const PDX12ComputeShader& shader)
{
	_bDirty = true;
	if (shader.GetCSData() != nullptr)
	{
		_desc.CS = {
			reinterpret_cast<byte*>(shader.GetCSData()->GetBufferPointer()),
			shader.GetCSData()->GetBufferSize()
		};
	}

}

bool PComputePipelineState::Finalize()
{
	if (_bDirty == false && _dx12PSO != nullptr)
	{
		return true;
	}

	uint64 hash = HHash::HashState(&_desc);

	ID3D12PipelineState** PSORef = nullptr;
	bool bFirstCompile = false;
	{
		static std::mutex s_HashMapMutex;
		std::lock_guard<std::mutex> CS(s_HashMapMutex);
		auto iter = HDirectXAPI::GetComputePSOCacheRef().find(hash);

		if (iter == HDirectXAPI::GetComputePSOCache().end())
		{
			bFirstCompile = true;
			PSORef = HDirectXAPI::GetComputePSOCacheRef()[hash].GetAddressOf();
		}
		else
			PSORef = iter->second.GetAddressOf();
	}

	if (bFirstCompile)
	{
		HRESULT hr = HDirectXAPI::GetDevice()->CreateComputePipelineState(&_desc, IID_PPV_ARGS(&_dx12PSO));
		if (FAILED(hr))
		{
			return false;
			assert("failed Create Compute PSO");
		}
		HDirectXAPI::GetComputePSOCacheRef()[hash] = _dx12PSO.Get();
	}
	else
	{
		while (*PSORef == nullptr)
			std::this_thread::yield();
		_dx12PSO = *PSORef;
	}

	_bDirty = false;
	return true;
}