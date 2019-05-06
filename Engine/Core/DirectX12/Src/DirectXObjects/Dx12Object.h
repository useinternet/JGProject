#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"ShaderCommonDefines.h"
#include"Mesh.h"
#include"Material.h"
#include"DirectXToolKit/PSO.h"
#include"DirectXToolKit/Texture.h"

namespace Dx12
{
	class CommandList;
	class Camera;
	class Dx12Object;
	class ObjectFragments;
	
	class Dx12Object
	{
		friend ObjectFragments;
		using ObjectCBArray  = std::vector<ObjectCB>;
		using SkinnedCBArray = std::vector<SkinnedCB>;
		using ObjectFragmentsArray  = std::vector<std::shared_ptr<ObjectFragments>>;
		using TextureSRVHandleArray = std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>;
		using TextureArray = std::vector<Texture>;
	private:
		std::string m_Name;
		std::string m_ShaderPath;
		std::vector<EShaderStage> m_ShaderStage;
		EPreparedPSO              m_PreparedPSO;


		uint32_t m_InstanceCount;
		Mesh     m_Mesh;
		SkinnedCB            m_SkinnedCB;
		ObjectCBArray        m_ObjectCBDatas;
		ObjectFragmentsArray m_ObjectFragments;
		GraphicsPSO           m_PSO;
		TextureArray          m_Textures;
		TextureSRVHandleArray m_TextureSRVHandles;
		TextureArray          m_CubeTextures;
		TextureSRVHandleArray m_CubeTextureSRVHandles;
		bool m_BuildOK;
	public:
		Dx12Object(const std::string& name = "Dx12Object");
	public:
		const std::string& GetName() const {
			
			return m_Name;
		}
		const Mesh* GetMesh_c() const {
			return &m_Mesh;
		}
		Mesh* GetMesh() {
			return &m_Mesh;
		}
		uint32_t GetInstanceCount() const {
			return (uint32_t)m_ObjectFragments.size();
		}
		const std::string& GetShaderPath() const {
			return m_ShaderPath;
		}
	public:
		void SetMesh(const Mesh& mesh);
		void SetName(const std::string& name);
		ObjectFragments* AddObjectFragments();
		ObjectFragments* GetObjectFragments(uint32_t instanceID);
		void RemoveObjectFragments(ObjectFragments* obj);
		void SetSkinnedCB(const SkinnedCB& cb);
		void Build(const std::string& shaderpath,
			       const std::vector<EShaderStage>& shaderstage,
			       EPreparedPSO pso_version);

		void Update();
		void Draw(CommandList* commandList) const;

		void AddTexture(const Texture& texture);
		void AddTextures(const std::vector<Texture>& textures);
		void PopTexture(uint32_t numTexture = 1);
		void SetTexture(ETextureSlot slot, const Texture& texture);

		void AddCubeTexture(const Texture& texture);
		void AddCubeTextures(const std::vector<Texture>& textures);
		void PopCubeTexture(uint32_t numTexture = 1);
		void SetCubeTexture(ECubeTextureSlot slot, const Texture& texture);
	private:
		void Build();
	};
	class ObjectFragments
	{
		friend Dx12Object;
	private:
		Dx12Object* m_OwnerObject;
		std::string m_Name;
		uint32_t    m_InstanceID;
		Common::Transformation    m_Transform;
		std::unique_ptr<Material> m_Material;
	public:
		ObjectFragments(Dx12Object* obj, uint32_t InstanceID) :
			m_OwnerObject(obj),
			m_InstanceID(InstanceID),
			m_Name(obj->m_Name + std::to_string(InstanceID))
		{
			m_Material = std::make_unique<Material>();
		}

		const std::string& GetName() const {
			return m_Name;
		}
		uint32_t GetInstanceID() const {
			return m_InstanceID;
		}
		Material* GetMaterial() {
			return m_Material.get();
		}
		const Material* GetMaterial_c() const {
			return m_Material.get();
		}
		Common::Transformation& GetTransform() {
			return m_Transform;
		}
		const Common::Transformation& GetTransform_c() const{
			return m_Transform;
		}
		Common::Location& GetLocation() {
			return m_Transform._Location();
		}
		const Common::Location& GetLocation() const {
			return m_Transform._Location_c();
		}
		Common::Rotator& GetRotation() {
			return m_Transform._Rotation();
		}
		const Common::Rotator& GetRotation_c() const {
			return m_Transform._Rotation_c();
		}
		Common::Scale& GetScale() {
			return m_Transform._Scale();
		}
		const Common::Scale& GetScale() const {
			return m_Transform._Scale_c();
		}
	};
}