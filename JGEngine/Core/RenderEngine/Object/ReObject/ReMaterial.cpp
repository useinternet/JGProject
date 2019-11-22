#include "pch.h"
#include "ReMaterial.h"
#include "Object/Shader/ShaderModule.h"
#include "Object/DxObject/PipelineState.h"
using namespace std;

namespace RE
{
	ReMaterial::ReMaterial()
	{
		m_PSO = make_shared<GraphicsPipelineState>();
	

	}

	void ReMaterial::SetBlendState(const D3D12_BLEND_DESC& desc)
	{

	}
	void ReMaterial::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc)
	{

	}
	void ReMaterial::SetRasterizerState(const D3D12_RASTERIZER_DESC& desc)
	{

	}
	void ReMaterial::AddTexture(const std::string& name, const std::string& asset_texture_path)
	{

	}





}



