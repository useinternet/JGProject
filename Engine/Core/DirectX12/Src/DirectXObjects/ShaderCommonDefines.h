#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"DirectXToolKit/RootSigature.h"
#include"DirectXToolKit/PSO.h"

#define MAIN_SHADER_PATH    GLOBAL_SHADER_PATH("Main.hlsl")
#define SKY_BOX_SHADER_PATH GLOBAL_SHADER_PATH("SkyBox.hlsl")
namespace Dx12
{
	class Shader;
	class GraphicsShader;
	namespace CommonRootParam
	{
		enum {
			ObjectCB   = 0,
			PassCB, 
			MaterialCB,
			SkinnedCB,
			Texture,
			CubeMap,
		};
	}

	struct ObjectCB
	{
		DirectX::XMFLOAT4X4 World;

		ObjectCB() {
			DirectX::XMStoreFloat4x4(&World, DirectX::XMMatrixIdentity());
		}
	};
	struct PassCB
	{
		DirectX::XMFLOAT4X4 ViewProj;
		DirectX::XMFLOAT3   ToEye = { 0.0f,0.0f,0.0f };
		float DeltaTime = 0.0f;
		PassCB() {
			DirectX::XMStoreFloat4x4(&ViewProj, DirectX::XMMatrixIdentity());
		}
	};
	struct SkinnedCB
	{
		DirectX::XMFLOAT4X4 Offset[96];

		SkinnedCB() {
			for (int i = 0; i < 96; ++i)
			{
				DirectX::XMStoreFloat4x4(&Offset[i], DirectX::XMMatrixIdentity());
			}
		}
	};


	class Dx12CommonShaderDefines
	{
	private:
		RootSignature m_MainRootSignature;
	public:
		Dx12CommonShaderDefines();
	public:
		const RootSignature& GetMainRootSig() const {
			return m_MainRootSignature;
		}
		GraphicsPSO GetMainPSO(const GraphicsShader& shader);
		GraphicsPSO GetMainSkinnedPSO(const GraphicsShader& shader);
	};
}