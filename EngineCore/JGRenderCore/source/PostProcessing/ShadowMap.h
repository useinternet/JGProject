#pragma once
#include"DxCommon/DxCommon.h"
namespace JGRC
{
	enum class ELightExercise;
	enum class EShadowMapType
	{
		Texture,
		Cube
	};
	class RCORE_EXPORT ShadowMap
	{
	private:
		static UINT m_Width;
		static UINT m_Height;
		static D3D12_VIEWPORT m_Viewport;
		static D3D12_RECT     m_ScissorRect;
	private:
		EShadowMapType m_Type;
		DXGI_FORMAT m_Format = DXGI_FORMAT_R24G8_TYPELESS;
		DXGI_FORMAT m_TextureFormat = DXGI_FORMAT_R8_UNORM;
	private:
		D3D12_RESOURCE_DESC   m_DefaultResourceDesc;
		D3D12_SHADER_RESOURCE_VIEW_DESC m_SrvDesc;
		D3D12_DEPTH_STENCIL_VIEW_DESC   m_DsvDesc;
	private: // Direction, Spot  ±×¸²ÀÚ ¸Ê
		DepthStencilViewPack* m_DepthStencilViewPack = nullptr;
		ID3D12Resource*       m_ShadowMap     = nullptr;
		SrvResourcePack*      m_ShadowSrvPack = nullptr;
		PassData*             m_ShadowPass    = nullptr;
	private: // point ±×¸²ÀÚ ¸Ê
		DepthStencilViewPack* m_CubeDepthStencilViewPack[6];
		PassData*             m_CubeShadowPass[6];
		//
	public:
		ShadowMap()  = default;
		~ShadowMap() = default;
	public:
		ShadowMap(UINT width, UINT height);
		void UpdateShadowPass(FrameResource* CurrentFrameResource, class JGLight* light);
		void BuildShadowMap(const std::string& name, EShadowMapType type);
		void Draw(FrameResource* CurrentFrameResource, ID3D12GraphicsCommandList* CommandList, ELightExercise LightExcType);
		static UINT GetWidth() { return m_Width; }
		static UINT GetHeight() { return m_Height; }
		UINT GetSrvIndex() const;
	private:
		void BuildResource();
		void BuildDescriptor(const std::string& name);
	};

}