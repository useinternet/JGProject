#pragma once
#include"DxCommon/DxCommon.h"
namespace JGRC
{
	
	/*
	Exp : ShaderResourceView Pack 
	* 리소스
	* 리소스 목록 */
	struct SrvResourcePack
	{
		std::string PackName;
		ID3D12Resource* Resource = nullptr;
		D3D12_SHADER_RESOURCE_VIEW_DESC Desc;
	};
	/*
	Exp : UnorderedAccessView Pack 
	* 리소스
	* 리소스 목록 */
	struct UavResourcePack
	{
		std::string PackName;
		ID3D12Resource* Resource = nullptr;
		ID3D12Resource* CounterResource = nullptr;
		D3D12_UNORDERED_ACCESS_VIEW_DESC Desc;
	};
	/*
	Exp : 텍스쳐 타입 
	* 기본
	* 큐브  */
	enum class ETextureType
	{
		Default,
		Cube
	};
	/*
	텍스쳐 팩 
	* 텍스쳐 데이터 
	* 텍스쳐 타입 */
	struct TexturePack
	{
		std::wstring Path;
		std::shared_ptr<Texture> Texture;
		ETextureType Type;
	};
	/*
	리소스 플래그 팩 */
	struct ResourceFlagPack
	{
		D3D12_HEAP_PROPERTIES Property = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		D3D12_HEAP_FLAGS      Flags    = D3D12_HEAP_FLAG_NONE;
		D3D12_RESOURCE_STATES State    = D3D12_RESOURCE_STATE_COMMON;
	};

	/*
	Exp : RederTarget Pack */
	struct RenderTargetPack
	{
		std::string PackName;
		ID3D12Resource* RenderResource = nullptr;
		D3D12_RENDER_TARGET_VIEW_DESC Desc;
		CD3DX12_CPU_DESCRIPTOR_HANDLE Handle;
		bool bDefault = false;
	};
	/*
	Exp : DepthStencilView Pack */
	struct DepthStencilViewPack
	{
		std::string PackName;
		ID3D12Resource* RenderResource = nullptr;
		D3D12_DEPTH_STENCIL_VIEW_DESC Desc;
		CD3DX12_CPU_DESCRIPTOR_HANDLE Handle;
		bool bDefault = false;
	};
	class RCORE_EXPORT ResourceManager
	{
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_Resources;
		std::unordered_map<ID3D12Resource*, D3D12_RESOURCE_STATES> m_ResourceStates;
	private:
		/*
		* Texture 배열
		* ShaderResourceView  배열
		* UnorderedAccessView 배열 */
		std::unordered_map<std::wstring, TexturePack>         m_Textures;
		std::unordered_map<std::wstring, TexturePack>         m_CubeTextures;
		std::unordered_map<std::string, SrvResourcePack>      m_SrvResources;
		std::unordered_map<std::string, SrvResourcePack>      m_CubeSrvResources;
		std::unordered_map<std::string, UavResourcePack>      m_UavResources;
		std::unordered_map<std::string, RenderTargetPack>     m_RenderTargets;
		std::unordered_map<std::string, DepthStencilViewPack> m_DepthStencilViews;
		/*
		* Texture Offset  배열
		* Srv,Uav Offset  배열  */
		std::unordered_map<std::wstring, UINT> m_TextureOffsets;
		std::unordered_map<std::string,  UINT> m_SrvUavOffsets;
		std::unordered_map<std::string,  UINT> m_RtvOffsets;
		std::unordered_map<std::string,  UINT> m_DsvOffsets;
		std::unordered_map<std::wstring, UINT> m_CubeTextureOffsets;
		std::unordered_map<std::string,  UINT> m_CubeMapOffsets;
		/*
		*  힙 ( srv, uav, texture 등을 저장하고있는 공간 배열 ) */
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvDescriptorHeap;
		/*
		* 각종 필요 포인터 및 변수들 */
		ID3D12Device* m_Device = nullptr;
		class DxCore* m_Core = nullptr;
		// 오프셋 저장 변수 
		UINT m_SrvHeapOffset  = 0;
		UINT m_CubeHeapOffset = 0;
		UINT m_RtvHeapOffset  = 0;
		UINT m_DsvHeapOffset  = 0;
	public:
		void Init(class DxCore* core);
	public:
		ID3D12Resource* BuildResource(D3D12_RESOURCE_DESC* desc, const ResourceFlagPack& Pack = ResourceFlagPack(), D3D12_CLEAR_VALUE* ClearValue = nullptr);
		ID3D12Resource* ReBuildResource(ID3D12Resource* resource, D3D12_RESOURCE_DESC* desc, const ResourceFlagPack& Pack = ResourceFlagPack(), D3D12_CLEAR_VALUE* ClearValue = nullptr);
	public:
		/*
		Exp : 텍스쳐, SRV, UAV 등 추가 해주는 함수 */
		TexturePack* AddTexture(ID3D12GraphicsCommandList* CommandList, const std::wstring& path, ETextureType type = ETextureType::Default);
		SrvResourcePack* AddSrv(const std::string& name, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* Desc);
		UavResourcePack* AddUav(const std::string& name, ID3D12Resource* resource, ID3D12Resource* Counterresource, D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc);
		RenderTargetPack* AddRtv(const std::string& name, ID3D12Resource* RenderResource, D3D12_RENDER_TARGET_VIEW_DESC* Desc);
		DepthStencilViewPack* AddDsv(const std::string& name, ID3D12Resource* RenderResource, D3D12_DEPTH_STENCIL_VIEW_DESC* Desc);
	public:
		SrvResourcePack* SetSrv(const std::string& name, ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* Desc = nullptr);
		UavResourcePack* SetUav(const std::string& name, ID3D12Resource* resource, ID3D12Resource* Counterresource, D3D12_UNORDERED_ACCESS_VIEW_DESC* Desc = nullptr);
		RenderTargetPack* SetRtv(const std::string& name, ID3D12Resource* RenderResource, D3D12_RENDER_TARGET_VIEW_DESC* Desc = nullptr);
		DepthStencilViewPack* SetDsv(const std::string& name, ID3D12Resource* RenderResource, D3D12_DEPTH_STENCIL_VIEW_DESC* Desc = nullptr);
	public:
		void BuildResourceHeap();
	public:
		/*
		Exp : 각종 리소스 및 텍스쳐  Get접근자. */
		Texture* GetTexture(const std::wstring& path);
		ID3D12Resource* GetSrv(const std::string& name);
		ID3D12Resource* GetUrv(const std::string& name);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtvHandle(const std::string& name);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsvHandle(const std::string& name);
	public: 
		/*
		Exp : 팩 Get 접근자. */
		TexturePack* GetTexturePack(const std::wstring& path);
		SrvResourcePack* GetSrvPack(const std::string& name);
		UavResourcePack* GetUavPack(const std::string& name);
		RenderTargetPack* GetRtvPack(const std::string& name);
		DepthStencilViewPack* GetDsvPack(const std::string& name);
	public:
		/*
		Exp : Heap Offset Get 접근자 */
		UINT GetTextureIndex(const std::wstring& path);
		UINT GetSrvUavIndex(const std::string& name);
		UINT GetRtvIndex(const std::string& name);
		UINT GetDsvIndex(const std::string& name);
		UINT GetCubeMapShaderIndex(const std::string& name);
		UINT GetCubeTextureShaderIndex(const std::wstring& name);
	public:
		/*
		Exp : Heap Handle */
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUSrvUavHandle(const UINT HeapIndex);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUSrvUavHandle(const UINT HeapIndex);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPURtvHandle(UINT HeapIndex);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUDsvHandle(UINT HeapIndex);
		/*
		Exp : TextureCube , Texture2D 시작점 Handle */
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUTexture2DHandle();
		CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUCubeMapHandle();
	public:
		/*
		Exp : DescriptorHeap 접근자 */
		ID3D12DescriptorHeap* SrvHeap() { return m_SrvDescriptorHeap.Get(); }
	public:
		UINT GetTextureCount() { return (UINT)m_Textures.size(); }
	public:
		void Clear();
		bool ResourceStateTransition(ID3D12GraphicsCommandList* CommandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES State);
	private:
		/*
		EXP : 텍스쳐 목록 작성 */
		static D3D12_SHADER_RESOURCE_VIEW_DESC GetDesc(TexturePack* t);
	};

}