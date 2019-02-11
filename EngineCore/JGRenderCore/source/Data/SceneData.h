#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	class RCORE_EXPORT SceneData
	{
		static UINT Count;
	private:
		SceneData(const SceneData& rhs) = delete;
		SceneData& operator=(const SceneData& SceneData) = delete;
	private:
		const DXGI_FORMAT m_WorldPosFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		const DXGI_FORMAT m_AlbedoFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
		const DXGI_FORMAT m_NormalFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
		const DXGI_FORMAT m_DepthFormat    = DXGI_FORMAT_R24G8_TYPELESS;
		const DXGI_FORMAT m_MatFormat      = DXGI_FORMAT_R32G32_UINT;

		D3D12_VIEWPORT m_ViewPort;
		D3D12_RECT     m_ScissorRect;
		std::string    m_Name = "SceneData";
	private:
		D3D12_RESOURCE_DESC m_WorldPosDesc;
		D3D12_RESOURCE_DESC m_AlbedoDesc;
		D3D12_RESOURCE_DESC m_NormalDesc;
		D3D12_RESOURCE_DESC m_MatDesc;
		D3D12_RESOURCE_DESC m_DepthDesc;
		ResourceFlagPack    m_ResourceFlag;
	private: // °¢ ¸®¼Ò½º
		ID3D12Resource* m_WorldPosResource = nullptr;
		ID3D12Resource* m_AlbedoResource   = nullptr;
		ID3D12Resource* m_NormalResource   = nullptr;
		ID3D12Resource* m_MatResource      = nullptr;
		ID3D12Resource* m_DepthResource    = nullptr;
	
	private: // °¢ ¸®¼Ò½º ÆÑ
		SrvResourcePack* m_WorldPosPack    = nullptr;
		SrvResourcePack* m_AlbedoPack      = nullptr;
		SrvResourcePack* m_NormalPack      = nullptr;
		SrvResourcePack* m_MatPack         = nullptr;
		SrvResourcePack* m_DepthPack       = nullptr;

	private: // ·»´õ Å¸°Ù
		RenderTargetPack* m_WorldPosRtv    = nullptr;
		RenderTargetPack* m_AlbedoRtv      = nullptr;
		RenderTargetPack* m_NormalRtv      = nullptr;
		RenderTargetPack* m_MatRtv         = nullptr;
		DepthStencilViewPack* m_DepthDsv   = nullptr;

	private:
		UINT m_Width  = 0;
		UINT m_Height = 0;
	public:
		SceneData(); 
		~SceneData() = default;
	public:
		void BuildSceneData(UINT width, UINT height);
		void ReSize(UINT width, UINT height);
		void SceneDataExtract(FrameResource* CurrFrameResource, ID3D12GraphicsCommandList* CommandList);
	public:
		ID3D12Resource* GetWorldPosResourec()    const { return m_WorldPosResource; }
		ID3D12Resource* GetAlbedoResource()      const { return m_AlbedoResource; }
		ID3D12Resource* GetNormalResource()      const { return m_NormalResource; }
		ID3D12Resource* GetMatReosurce()         const { return m_MatResource; }
		ID3D12Resource* GetDepthResource()       const { return m_DepthResource; }
		UINT GetWorldPosIndex() const;
		UINT GetAlbedoIndex()   const;
		UINT GetNormalIndex()   const;
		UINT GetMatIndex()      const;
		UINT GetDepthIndex()    const;
	private:
		void BuildResource();
		void ReBuildResource();
		void RegisterHeap();
		void ReRegisterHeap();
	};
}
