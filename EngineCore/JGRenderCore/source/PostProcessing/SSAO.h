#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	struct SSAOData
	{
		DirectX::XMFLOAT4X4 Proj;
		DirectX::XMFLOAT4X4 InvProj;
		DirectX::XMFLOAT4X4 ProjTex;
		DirectX::XMFLOAT4 OffsetVectors[14];
		DirectX::XMFLOAT4 BlurWeights[3];
		DirectX::XMFLOAT2 InvRenderTargetSize;

		float    OcclusionRadius;
		float    OcclusionFadeStart;
		float    OcclusionFadeEnd;
		float    SurfaceEpsilon;
	};
	class RCORE_EXPORT SSAO
	{
		// 
		static const int MaxBlurRadius = 5;
		static const DXGI_FORMAT SSAOMapFormat = DXGI_FORMAT_R16_UNORM;
		static const DXGI_FORMAT NormalMapFormat  = DXGI_FORMAT_R16G16B16A16_FLOAT;
	private:
		SSAO& operator=(const SSAO& rhs) = delete;
		SSAO(const SSAO& rhs)            = delete;

		UINT m_Width  = 0;
		UINT m_Height = 0;
		D3D12_RESOURCE_DESC m_NormalDesc;
		D3D12_RESOURCE_DESC m_SSAODesc;
		ID3D12Resource*   m_NormalMap        = nullptr;
		SrvResourcePack*  m_NormalMapPack    = nullptr;
		RenderTargetPack* m_NormalMapRtvPack = nullptr;
		std::shared_ptr<class Shader> m_NormalMapShader;
		ID3D12PipelineState* m_NormalMapPSO  = nullptr;
		//
		SrvResourcePack*  m_DepthMapPack = nullptr;
		//
		ID3D12Resource*   m_SSAOMap_0 = nullptr;
		SrvResourcePack*  m_SSAOMapPack_0 = nullptr;
		RenderTargetPack* m_SSAORtvPack_0 = nullptr;
		//
		ID3D12Resource*   m_SSAOMap_1 = nullptr;
		SrvResourcePack*  m_SSAOMapPack_1 = nullptr;
		RenderTargetPack* m_SSAORtvPack_1 = nullptr;
		//
		ID3D12PipelineState* m_SSAOPSO     = nullptr;
		ID3D12PipelineState* m_SSAOBlurPSO = nullptr;
		std::shared_ptr<class Shader> m_SSAOShader;
		std::shared_ptr<class Shader> m_SSAOBlurShader;
		std::shared_ptr<class SSAOShaderRootSignature> m_SSAORootSig;
		//
		ID3D12Resource* m_RandomVectorMap = nullptr;
		SrvResourcePack* m_RandomVectorMapPack = nullptr;

		ID3D12Resource* m_RandomVectorMapUploadBuffer = nullptr;
		DirectX::XMFLOAT4 m_Offsets[14];

		std::unique_ptr<UploadBuffer<SSAOData>> m_SSAOCB;

		//
		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;
		D3D12_VIEWPORT m_SSAOViewport;
		D3D12_RECT m_SSAOScissorRect;
	public:
		SSAO();
		~SSAO() = default;
		void BuildSSAO(UINT width, UINT height, ID3D12GraphicsCommandList* CommandList,class  CommonShaderRootSignature* RootSig);
		void OnReSize(UINT width, UINT height);
		void Update(FrameResource* CurrFrameResource);
		void Draw(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList);
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetNormalMapGPUHandle();
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetDepthMapGPUHandle();
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetRandomVectorGPUHandle();
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetSSAOMapGPUHandle_0();
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetSSAOMapGPUHandle_1();
		UINT GetShaderIndex();
	private:
		void BuildResource(ID3D12GraphicsCommandList* CommandList);
		void ReBuildResource();

		void BuildDescriptor();
		void ReBuildDescrptor();
		void DrawNormalDepthMap(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList);
		void DrawSSAO(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList);
		void BlurSSAOMap(ID3D12GraphicsCommandList* CommandList, FrameResource* CurrFrameResource, int blurCount);
		void BlurSSAOMap(ID3D12GraphicsCommandList* CommandList, bool horzBlur);
		void BuildOffsetVectors();


	private:
		std::vector<float> CalcGaussWeights(float sigma);

	};
}