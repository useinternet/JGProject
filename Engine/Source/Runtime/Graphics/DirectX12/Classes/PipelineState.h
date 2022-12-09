#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"
#include "DirectX12Helper.h"

class PRootSignature;
class PDX12GraphicsShader;
class PDX12ComputeShader;

class PPipelineState : public IMemoryObject
{
protected:
	HDX12ComPtr<HDX12Pipeline> _dx12PSO;
	bool _bDirty = true;
public:
	virtual ~PPipelineState() = default;

	HDX12Pipeline* Get() const {
		return _dx12PSO.Get();
	}

	virtual bool Finalize() = 0;
};

class PGraphicsPipelineState : public PPipelineState
{
private:
	D3D12_GRAPHICS_PIPELINE_STATE_DESC _desc = {};
	HList<D3D12_INPUT_ELEMENT_DESC> _inputLayoutDescs;
public:
	virtual ~PGraphicsPipelineState() = default;
public:
	PGraphicsPipelineState();
	void BindRootSignature(const PRootSignature& rootSig);
	void BindRenderTarget(const HList<DXGI_FORMAT>& rtFormats, DXGI_FORMAT dvFormat = DXGI_FORMAT_UNKNOWN);
	void BindInputLayout(const HInputLayout& inputLayout);
	void BindShader(const PDX12GraphicsShader& shader);
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
	void SetSampleMask(uint32_t sampleMask);
	void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
	void SetBlendState(const D3D12_BLEND_DESC& desc);
	void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);

	virtual bool Finalize() override;

	const D3D12_BLEND_DESC& GetBlendDesc() const;
};

class PComputePipelineState : public PPipelineState
{
private:
	D3D12_COMPUTE_PIPELINE_STATE_DESC _desc = {};
public:
	virtual ~PComputePipelineState() = default;
public:
	PComputePipelineState();
	void BindRootSignature(const PRootSignature& rootSig);
	void BindShader(const PDX12ComputeShader& shader);


	virtual bool Finalize() override;
};