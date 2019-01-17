#pragma once
#include"DxCommon/DxCommon.h"


namespace JGRC
{
	struct BlurData
	{
		DirectX::XMFLOAT4X4 Proj;
		DirectX::XMFLOAT4 BlurWeight[3];
		DirectX::XMFLOAT2 InvRenderTargetSize;
		float Pad[2];
	};
	class RCORE_EXPORT BlurFilter
	{
		static UINT Count;
	private:
		BlurFilter(const BlurFilter& rhs) = delete;
		BlurFilter& operator=(const BlurFilter& rhs) = delete;
	private:
		UINT m_Width  = 0;
		UINT m_Height = 0;
		// ¼ÎÀÌ´õ
		class ShaderRootSignatureBase* m_PrevRootSig = nullptr;
		std::shared_ptr<class BlurShaderRootSignature> m_BlurRootSig;
		std::shared_ptr<class Shader> m_BlurShader;
		ID3D12PipelineState* m_BlurPSO = nullptr;
		// ·»´õ Å¸°Ù¹× ¸®¼Ò½º
		ID3D12Resource*   m_BlurResource  = nullptr;
		SrvResourcePack*  m_BlurPack     = nullptr;
		RenderTargetPack* m_BlurRtvPack = nullptr;
		//
		SrvResourcePack*  m_OwnerSrvPack = nullptr;
		RenderTargetPack* m_OwnerRtvPack = nullptr;

		std::shared_ptr<UploadBuffer<BlurData>> m_BlurCB;
	public:
		BlurFilter()  = default;
		~BlurFilter() = default;

		void BuildFilter(UINT width, UINT height, SrvResourcePack* OwnerSrvPack,
			RenderTargetPack* OwnerRtvPack);
		void ReSize(UINT width, UINT height, SrvResourcePack* OwnerSrvPack, RenderTargetPack* OwnerRtvPack);
		void Update();
		void ExcuteBlur(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, UINT BlurCount);
	private:
		void BuildResource();
		void ReBuildResource();
		void RegisterDescriptor();
		void ReRegisterDescriptor();
		std::vector<float> CalcGaussWeights(float sigma);
	};
}