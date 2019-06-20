#pragma once

#include"ShaderType.h"
#include"GraphicsResource.h"



namespace GR
{
	namespace Dx12
	{
		class Mesh;
		class Renderer;
		class GBuffer;
		class Material;
		class GraphicsCommander;
		class RenderObject
		{
			friend GBuffer;
			friend Renderer;
		private:
			std::vector<ObjectCB> m_ObjectCBs;
			std::shared_ptr<Mesh> m_Mesh;
			std::shared_ptr<Material> m_Material;
		public:
			RenderObject();
			void BindMesh(Mesh* mesh);
			void BindMaterial(class Material* mat);
			void BindConstantBufferAtGPU(GraphicsCommander* commander);
			void AddInstance(const ObjectCB& objectCB);
			void SetInstance(uint32_t ID, const ObjectCB& objectCB);
			ObjectCB& GetInstance(uint32_t ID);
			const ObjectCB& GetInstance(uint32_t ID) const;
			
			Mesh* GetMesh();
			Material* GetMaterial();
			const Mesh* GetMesh() const;
			const Material* GetMaterial() const;
		};
	}
}