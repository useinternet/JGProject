#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"DirectXToolKit/RootSigature.h"

// ºŒ¿Ã¥ı path
#define MAIN_SHADER_PATH     GLOBAL_SHADER_PATH("Main.hlsl")
#define SKY_BOX_SHADER_PATH  GLOBAL_SHADER_PATH("SkyBox.hlsl")
#define GENERATE_MIPMAP_PATH GLOBAL_SHADER_PATH("GenerateMips_CS.hlsl");
// Define ¡§¿«
#define SHADER_MACRO_USE_SKINNED "USE_SKINNED"

#define SHADER_MACRO_USE_TEXTURE_SLOT(x) "USE_TEXTURE_SLOT" + std::to_string(x)
#define SHADER_MACRO_USE_TEXTURE_SLOT0   "USE_TEXTURE_SLOT0"
#define SHADER_MACRO_USE_TEXTURE_SLOT1   "USE_TEXTURE_SLOT1"
#define SHADER_MACRO_USE_TEXTURE_SLOT2   "USE_TEXTURE_SLOT2"
#define SHADER_MACRO_USE_TEXTURE_SLOT3   "USE_TEXTURE_SLOT3"
#define SHADER_MACRO_USE_TEXTURE_SLOT4   "USE_TEXTURE_SLOT4"
#define SHADER_MACRO_USE_TEXTURE_SLOT5   "USE_TEXTURE_SLOT5"
#define SHADER_MACRO_USE_TEXTURE_SLOT6   "USE_TEXTURE_SLOT6"
#define SHADER_MACRO_USE_TEXTURE_SLOT7   "USE_TEXTURE_SLOT7"

#define SHADER_MACRO_USE_CUBETEXTURE_SLOT(x) "USE_CUBETEXTURE_SLOT" + std::to_string(x)
#define SHADER_MACRO_USE_CUBETEXTURE_SLOT0   "USE_CUBETEXTURE_SLOT0"
#define SHADER_MACRO_USE_CUBETEXTURE_SLOT1   "USE_CUBETEXTURE_SLOT1"
#define SHADER_MACRO_USE_CUBETEXTURE_SLOT2   "USE_CUBETEXTURE_SLOT2"
#define SHADER_MACRO_USE_CUBETEXTURE_SLOT3   "USE_CUBETEXTURE_SLOT3"











#define SHADER_MACRO_DEFINE "1"
#define SHADER_MACRO_UNDEFINE "0"

namespace Dx12
{
	class Shader;
	class GraphicsPSO;
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
		int MaterialIndex;
		int padding[3];
		ObjectCB() {
			MaterialIndex = 0;
			DirectX::XMStoreFloat4x4(&World, DirectX::XMMatrixIdentity());
		}
	};
	struct MaterialCB
	{
		DirectX::XMFLOAT3 SurfaceColor;
		float Roughness;
		float Metalic;
		float BlankParam[3];

		MaterialCB() :
			SurfaceColor({ 1.0f,1.0f,1.0f }), 
			Roughness(0.0f),
			Metalic(0.0f)
		{
			BlankParam[0] = 0.0f; BlankParam[1] = 0.0f; BlankParam[2] = 0.0f;
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
	typedef int EPreparedPSO;
	namespace PreparedPSO
	{
		enum
		{
			Main_Static,
			Main_Skeletal,
			SkyBox
		};
	}
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
		GraphicsPSO GetPSO(EPreparedPSO pso, const GraphicsShader& shader);
		GraphicsPSO GetMainPSO(const GraphicsShader& shader, bool is_skinned = false);
		GraphicsPSO GetSkyBoxPSO(const GraphicsShader& shader);
	};
}