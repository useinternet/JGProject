#pragma once
#include "Object/ReObject.h"

namespace RE
{
	class GraphicsShaderModule;
	class GraphicsPipelineState;
	enum class EReMaterialUsage
	{
		GUI,
		Static3D,
		Skeletal3D
	};
	class ReMaterial : public ReObject
	{
	public:
		GraphicsPipelineState* GetPSO() const {
			return m_PSO.get();
		}
		ReMaterial();
	public:
		void SetBlendState(const D3D12_BLEND_DESC& desc);
		void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& desc);
		void SetRasterizerState(const D3D12_RASTERIZER_DESC& desc);
		void AddTexture(const std::string& name, const std::string& asset_texture_path);
	private:
		std::shared_ptr<GraphicsPipelineState> m_PSO;
		EReMaterialUsage m_Usage = EReMaterialUsage::Static3D;
	};


	
}