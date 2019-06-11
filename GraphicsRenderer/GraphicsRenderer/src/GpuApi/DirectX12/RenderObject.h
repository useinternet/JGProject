#pragma once

#include"ShaderType.h"
#include"GraphicsResource.h"



namespace GR
{
	namespace Dx12
	{
		class Mesh;
		class Renderer;
		class RenderObject
		{
			friend Renderer;
		public:
			enum ETextureSlot
			{
				ALBEDO,
				NORMAL,
				SPECULAR,
				METALLIC,
				ROUGHNESS,
				A0,
				CUSTOM0,
				CUSTOM1,
				NUM_TEXTURESLOT,

				//  ½½·Ô
				SLOT1 = ALBEDO,
				SLOT2 = NORMAL,
				SLOT3 = SPECULAR,
				SLOT4 = METALLIC,
				SLOT5 = ROUGHNESS,
				SLOT6 = A0,
				SLOT7 = CUSTOM0,
				SLOT8 = CUSTOM1,

				// PBR
				SPECULARBRDF  = SLOT7,
				SPECULARMAP   = SLOT1,
				IRRADIANCEMAP = SLOT2
			};
		private:
			std::vector<ObjectCB> m_ObjectCBs;
			Texture m_Textures[NUM_TEXTURESLOT];
			Texture m_CubeTextures[NUM_TEXTURESLOT];
			std::shared_ptr<Mesh> m_Mesh;
		public:
			RenderObject();
			void BindMesh(Mesh* mesh);
			void BindTexture(ETextureSlot slot, const Texture& texture);
			void BindCubeTexture(ETextureSlot slot, const Texture& texture);
			void AddInstance(const ObjectCB& objectCB);
			void SetInstance(uint32_t ID, const ObjectCB& objectCB);
			ObjectCB& GetInstance(uint32_t ID);
			const ObjectCB& GetInstance(uint32_t ID) const;

			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> GetTextureSRV() const;
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> GetCubeTextureSRV() const;
		};
	}
}