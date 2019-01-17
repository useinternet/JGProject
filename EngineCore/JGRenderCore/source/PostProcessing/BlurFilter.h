#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	struct BlurFilterDesc
	{
		DXGI_FORMAT Format;
		D3D12_RESOURCE_DESC  ResourceDesc;
		D3D12_SHADER_RESOURCE_VIEW_DESC  SrvDesc;
		D3D12_UNORDERED_ACCESS_VIEW_DESC UavDesc;
	};
	class RCORE_EXPORT BlurFilter
	{
		const int MaxBlurRadius = 5;
		static UINT Count;
	private:
		std::string m_Name = "BlurMap";
		UINT m_Width = 0;
		UINT m_Height = 0;
		DXGI_FORMAT m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		ID3D12Resource* m_BlurMap0 = nullptr;
		ID3D12Resource* m_BlurMap1 = nullptr;
		D3D12_SHADER_RESOURCE_VIEW_DESC  m_SrvDesc = {};
		D3D12_UNORDERED_ACCESS_VIEW_DESC m_UavDesc = {};
		D3D12_RESOURCE_DESC m_ResourceDesc;
		std::unique_ptr<class BlurShaderRootSignature> m_RootSig;
		std::unique_ptr<class ComputeShader> m_Shader;
	public:
		BlurFilter(UINT width, UINT height);
		BlurFilter(UINT width, UINT height, const BlurFilterDesc& desc);
		virtual ~BlurFilter();
		void OnReSize(UINT newWidth, UINT newHeight);
		void Execute(ID3D12Resource* input, ID3D12GraphicsCommandList* CommandList, 
			D3D12_RESOURCE_STATES CurrState, int blurCount);
		ID3D12Resource* Output();
	private:
		void BuildDescriptors();
		void BuildResource();
		std::vector<float> CalcGaussWeights(float sigma);
	};
}