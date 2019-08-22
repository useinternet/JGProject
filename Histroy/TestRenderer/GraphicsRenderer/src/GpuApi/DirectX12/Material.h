#pragma once
#include"Dx12Include.h"
#include"GraphicsResource.h"
#include"PipelineState.h"
#include<string>


#define USE_SLOT0 "USE_SLOT0"
#define USE_SLOT1 "USE_SLOT1"
#define USE_SLOT2 "USE_SLOT2"
#define USE_SLOT3 "USE_SLOT3"
#define USE_SLOT4 "USE_SLOT4"
#define USE_SLOT5 "USE_SLOT5"
#define USE_SLOT6 "USE_SLOT6"
#define USE_SLOT7 "USE_SLOT7"

#define USE_SLOT(x) ("USE_SLOT" + std::to_string(x))

#define USE_CUBESLOT0 "USE_CUBESLOT0"
#define USE_CUBESLOT1 "USE_CUBESLOT1"
#define USE_CUBESLOT2 "USE_CUBESLOT2"
#define USE_CUBESLOT3 "USE_CUBESLOT3"
#define USE_CUBESLOT4 "USE_CUBESLOT4"
#define USE_CUBESLOT5 "USE_CUBESLOT5"
#define USE_CUBESLOT6 "USE_CUBESLOT6"
#define USE_CUBESLOT7 "USE_CUBESLOT7"

#define USE_CUBESLOT(x) ("USE_CUBESLOT" + std::to_string(x))

#define USE_IBL "USE_IBL"
#define USE_SKELETAL "USE_SKELETAL"

#define MACRO_DEFINE "1"
namespace GR
{
	namespace Dx12
	{
		class GraphicsCommander;
		enum class ETextureSlot
		{
			Slot0 = 0,
			Slot1,
			Slot2,
			Slot3,
			Slot4,
			Slot5,
			Slot6,
			Slot7,
			Num_Slot,

			// 일반 텍스쳐
			Albedo    = Slot0,
			Normal    = Slot1,
			Specular  = Slot2,
			Metallic  = Slot3,
			Roughness = Slot4,
			A0        = Slot5,
			SpecularBRDF = Slot6,

			// 큐브 맵
			CubeSlot0 = Slot0,
			CubeSlot1 = Slot1,
			CubeSlot2 = Slot2,
			CubeSlot3 = Slot3,
			CubeSlot4 = Slot4,
			CubeSlot5 = Slot5,
			CubeSlot6 = Slot6,
			CubeSlot7 = Slot7,

			SpecularMap   = Slot0,
			IrradianceMap = Slot1,
		};
		struct MaterialDesc
		{
			bool is_skeletal = false;
			bool operator==(const MaterialDesc& desc) {
				if (is_skeletal == desc.is_skeletal)
					return true;
				return false;
			}
			bool operator!=(const MaterialDesc& desc) {
				if (!operator==(desc))
					return true;
				return false;
			}
		};
		class Material
		{
		private:
			std::wstring m_ShaderVSName = L"GBufferVS.hlsl";
			std::wstring m_ShaderPSName = L"GBufferPS.hlsl";

			Texture m_Textures[(int)ETextureSlot::Num_Slot];
			Texture m_CubeTextures[(int)ETextureSlot::Num_Slot];
			MaterialDesc    m_MatDesc;



			GraphicsPSO m_PSO;
			bool m_isBuild;
		public:
			Material();
			void BindTexture(ETextureSlot slot, const Texture& texture);
			void BindCubeTexture(ETextureSlot slot, const Texture& texture);
			void SetShader(const std::wstring& vsfileName, const std::wstring& psfileName);
			void SetDesc(const MaterialDesc& desc);
			void RequestRebuild();
			void BindResourceAtGPU(GraphicsCommander* commander);
		public:
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> GetTextureSRV() const;
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> GetCubeTextureSRV() const;
			GraphicsPSO& GetPSO();
		public:
			void Build();
		};
	}
}