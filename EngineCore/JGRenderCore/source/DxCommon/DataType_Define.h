#pragma once
#include"DxCommon.h"

namespace JGRC
{
	template<class T>
	class ShaderDataType
	{
	private:
		bool bIsDead = false;
		UINT CBIndex;
		UINT CBSize;
		T    Data;
	public:
		ShaderDataType(UINT index) : CBIndex(index) {
			CBSize = d3dUtil::CalcConstantBufferByteSize(sizeof(T));
		}
		T& Get()      { return Data; }
		UINT Index()  { return CBIndex; }
		UINT Size()   { return CBSize; }
		bool IsDead() { return bIsDead; }
		void Dead()   { bIsDead = true; }
		void Live()   { bIsDead = false; }

	};
	typedef struct ObjectConstantData
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
		UINT MaterialIndex = 0;
		UINT CubeMapIndex = 0;
	}ObjectConstantData;


	typedef struct InstanceConstantData
	{
		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
		UINT MaterialIndex = 0;
		UINT CubeMapIndex = 0;
		UINT ObjPad[2];
	}cbInstanceConstantData;
	typedef struct SkinnedConstantData
	{
		DirectX::XMFLOAT4X4 BoneTransforms[96];
	}SkinnedConstantData;
	typedef struct PassConstantData
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
		UINT SkyBoxIndex = 0;
		UINT DirLightCount = 0;
		UINT PointLightCount = 0;
		UINT SpotLightCount = 0;
		UINT NormalSceneIndex;
		UINT AlbedoSceneIndex;
		UINT WorldPosSceneIndex;
		UINT DepthSceneIndex;
		UINT MatSceneIndex;
		UINT Pad;
	}PassConstantData;




	typedef struct MaterialConstantData
	{
		DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f,1.0f,1.0f,1.0f };
		DirectX::XMFLOAT3 FresnelR0 = { 0.0f, 0.0f, 0.0f };
		float Roughness = 0.0f;
		float Metallic = 0.0f;
		float CustomPad[3] = { 0.0f,0.0f,0.0f };
		DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
		UINT TextureIndex[8];
	}MaterialConstantData;

	typedef ShaderDataType<ObjectConstantData>   ObjectData;
	typedef ShaderDataType<InstanceConstantData> InstanceData;
	typedef ShaderDataType<SkinnedConstantData>  SkinnedData;
	typedef ShaderDataType<PassConstantData>     PassData;
	typedef ShaderDataType<MaterialConstantData> MaterialData;
}
