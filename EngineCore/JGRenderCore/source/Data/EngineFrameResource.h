#pragma once
#include"DxCommon/DxCommon.h"


namespace JGRC
{
	typedef struct cbObjectConstant
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
		UINT MaterialIndex = 0;
		UINT CubeMapIndex = 0;
	}cbObjectConstant;
	typedef struct InstanceData
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
		UINT MaterialIndex = 0;
		UINT CubeMapIndex  = 0;
		UINT ObjPad[2];
	}cbInstanceData;
	typedef struct SkinnedData
	{
		DirectX::XMFLOAT4X4 BoneTransforms[96];
	}SkinnedData;
	typedef struct cbPassConstant
	{
		DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 ViewProjTex = MathHelper::Identity4x4();
		DirectX::XMFLOAT3 EyePosW = { 0.0f,0.0f,0.0f };
		UINT SSAOTexutreIndex = 0;
		DirectX::XMFLOAT2 RenderTargetSize = { 0.0f,0.0f };
		DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f,0.0f };
		float TotalTime = 0.0f;
		float DeltaTime = 0.0f;
		UINT SkyBoxIndex      = 0;
		UINT DirLightCount   = 0;
		UINT PointLightCount = 0;
		UINT SpotLightCount  = 0;
		UINT NormalSceneIndex;
		UINT AlbedoSceneIndex;
		UINT WorldPosSceneIndex;
		UINT DepthSceneIndex;
		UINT MatSceneIndex;
		UINT Pad;
	}cbPassConstant;
	typedef struct PassData
	{
		UINT PassCBIndex;
		cbPassConstant Data;
	}PassData;
	typedef struct MaterialData
	{
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f,1.0f,1.0f,1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.0f, 0.0f, 0.0f };
		float Roughness  = 0.0f;
		float Refractive = 1.0f;
		float Reflectivity = 1.0f;
		float CustomPad[2] = { 0.0f,0.0f };
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
		UINT TextureIndex[8];
	}MaterialData;

	class RCORE_EXPORT FrameResource
	{

	private:
		FrameResource(const FrameResource& rhs) = delete;
		FrameResource& operator=(const FrameResource& rhs) = delete;
	public:
		FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount);
		~FrameResource() {}
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator>   CmdListAlloc;
		std::unique_ptr<UploadBuffer<cbPassConstant>>    PassCB     = nullptr;
		std::unique_ptr<UploadBuffer<cbObjectConstant>>  ObjectCB   = nullptr;
		std::unique_ptr<UploadBuffer<MaterialData>>      MaterialCB = nullptr;
		std::unique_ptr<UploadBuffer<SkinnedData>>       SkinnedCB  = nullptr;
		std::unique_ptr<UploadBuffer<Light>>             LightCB    = nullptr;
		UINT64 Fence = 0;
	public:
		void ReSize(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount);
	};

	class RCORE_EXPORT EngineFrameResourceManager
	{
		typedef std::vector<std::shared_ptr<FrameResource>> EngineFrameResourceArray;
	private:
		ID3D12Device* m_Device = nullptr;
		EngineFrameResourceArray m_FrameResources;
		FrameResource* m_CurrentFrameResource = nullptr;
		UINT m_CurrentFrameResourceIndex = 0;
	public:
		void BuildFrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT lightCount);
		void Resize(UINT passCount, UINT objectCount,UINT materialCount, UINT lightCount);
		void FrameResourcePerFrame(ID3D12Fence* Fence);
	public:
		FrameResource* operator[](const UINT idx)   { return m_FrameResources[idx].get(); }
		UINT CurrentIndex() const                   { return m_CurrentFrameResourceIndex; }
		FrameResource* CurrentFrameResource() const { return m_CurrentFrameResource; }
	};

}