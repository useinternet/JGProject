#pragma once


#include "Material.h"
#include "DirectXApi.h"

namespace DX12
{
	class DX12_Material;
	class DX12_MaterialInstance : public GE::MaterialInstance
	{
		friend DX12_Material;
	public:
		virtual bool SetVarAsFloat(const std::string& name, float f) override;
		virtual bool SetVarAsFloat2(const std::string& name, const JVector2& v) override;
		virtual bool SetVarAsFloat3(const std::string& name, const JVector3& v) override;
		virtual bool SetVarAsFloat4(const std::string& name, const JVector4& v) override;
		virtual bool SetVarAsInt(const std::string& name, int i) override;
		virtual bool SetVarAsInt2(const std::string& name, const JVector2Int& i) override;
		virtual bool SetVarAsInt3(const std::string& name, const JVector3Int& i) override;
		virtual bool SetVarAsInt4(const std::string& name, const JVector4Int& i) override;
		virtual bool SetVarAsUint(const std::string& name, uint32_t i) override;
		virtual bool SetVarAsUint2(const std::string& name, const JVector2Uint& i) override;
		virtual bool SetVarAsUint3(const std::string& name, const JVector3Uint& i) override;
		virtual bool SetVarAsUint4(const std::string& name, const JVector4Uint& i) override;
		virtual bool SetVarAsMatrix4x4(const std::string& name, const JMatrix& m) override;

		virtual bool BindTexture(const std::string& name, GE::Texture* t) override;

		virtual bool GetVarAsFloat(const std::string& name, float& dataRef) const override;
		virtual bool GetVarAsFloat2(const std::string& name, JVector2& dataRef) const override;
		virtual bool GetVarAsFloat3(const std::string& name, JVector3& dataRef) const override;
		virtual bool GetVarAsFloat4(const std::string& name, JVector4& dataRef) const override;
		virtual bool GetVarAsInt(const std::string& name, int& dataRef) const override;
		virtual bool GetVarAsInt2(const std::string& name, JVector2Int& dataRef) const override;
		virtual bool GetVarAsInt3(const std::string& name, JVector3Int& dataRef) const override;
		virtual bool GetVarAsInt4(const std::string& name, JVector4Int& dataRef) const override;
		virtual bool GetVarAsUint(const std::string& name, uint32_t& dataRef) const override;
		virtual bool GetVarAsUint2(const std::string& name, JVector2Uint& dataRef) const override;
		virtual bool GetVarAsUint3(const std::string& name, JVector3Uint& dataRef) const override;
		virtual bool GetVarAsUint4(const std::string& name, JVector4Uint& dataRef) const override;
		virtual bool GetVarAsMatrix4x4(const std::string& name, JMatrix& dataRef) const override;


		virtual const char* BtData()  const override {
			return m_BtData.data();
		}
		virtual size_t BtSize() const override {
			return m_BtData.size();
		}
		virtual GE::Material* GetOwnerMaterial() const override
		{
			return m_OwnerMaterial;
		}
		virtual const std::string& GetErrorCode() const override {
			return m_ErrorCode;
		}
		virtual void GetTextureData(void* dataPtr) const override;
		virtual const std::vector<std::pair<std::string, GE::Texture*>>& GetBindedTextures() const override;
		virtual ~DX12_MaterialInstance() override {}
	private:
		struct DataBlock
		{
			std::string name;
			ShaderDataType type;
			uint32_t pos;
			uint32_t size;

		};

		template<typename T, ShaderDataType DataType>
		bool SetVarAs(const std::string& name, const T& data)
		{
			auto iter = std::find_if(m_DataBlocks.begin(), m_DataBlocks.end(), [&](const DataBlock& block) -> bool
			{
				return block.name == name;
			});
			if (iter == m_DataBlocks.end()) {

				m_ErrorCode = "DX12_Material::DefineVarAs : This variable does not exist.";
				return false;
			}
			auto& block = *iter;
			memcpy(&m_BtData[block.pos], &data, block.size);
			return true;
		}
		template<typename T, ShaderDataType DataType>
		bool GetVarAs(const std::string& name, T* data) const
		{
			auto iter = std::find_if(m_DataBlocks.begin(), m_DataBlocks.end(), [&](const DataBlock& block) -> bool
			{
				return block.name == name;
			});
			if (iter == m_DataBlocks.end()) {

				m_ErrorCode = "DX12_Material::GetVarAs : This variable does not exist.";
				return false;
			}


			auto& block = *iter;
			if (block.type != DataType)
			{
				m_ErrorCode = "DX12_Material::GetVarAs : This variable type  mismatch.";
				return false;
			}


			memcpy(data, &m_BtData[block.pos], block.size);
			return true;
		}
	private:
		using BindedTextureArray = std::vector<std::pair<std::string, GE::Texture*>>;
	private:
		std::vector<DataBlock> m_DataBlocks;
		std::vector<char>      m_BtData;
		BindedTextureArray     m_BindedTextures;
		mutable std::string    m_ErrorCode;
		GE::Material*          m_OwnerMaterial = nullptr;
	};





	REGISTER_GRAPHICS_INTERFACE_MATERIAL(DX12_Material)
	class DX12_Material : public GE::Material
	{
	public:
		DX12_Material() : GE::Material()
		{
			m_BtData.resize(SHADER_CONSTANT_BUFFER_UNIT_SIZE);
		}
	public:
		virtual void SetMaterialProperty(const GE::MaterialProperty& ppt) override;
		virtual const GE::MaterialProperty& GetMaterialProperty() const override;



		// 머터리얼 변수 선언
		virtual void SetCode(const std::string& code) override;
		virtual bool DefineVarAsFloat(const std::string& name, float f) override;
		virtual bool DefineVarAsFloat2(const std::string& name, const JVector2& v) override;
		virtual bool DefineVarAsFloat3(const std::string& name, const JVector3& v) override;
		virtual bool DefineVarAsFloat4(const std::string& name, const JVector4& v) override;
		virtual bool DefineVarAsInt(const std::string& name, int i) override;
		virtual bool DefineVarAsInt2(const std::string& name, const JVector2Int& i) override;
		virtual bool DefineVarAsInt3(const std::string& name, const JVector3Int& i) override;
		virtual bool DefineVarAsInt4(const std::string& name, const JVector4Int& i) override;
		virtual bool DefineVarAsUint(const std::string& name, uint32_t i) override;
		virtual bool DefineVarAsUint2(const std::string& name, const JVector2Uint& i) override;
		virtual bool DefineVarAsUint3(const std::string& name, const JVector3Uint& i) override;
		virtual bool DefineVarAsUint4(const std::string& name, const JVector4Uint& i) override;
		virtual bool DefineVarAsMatrix4x4(const std::string& name, const JMatrix& m) override;


		virtual void UnDefineVar(const std::string& name) override;

		virtual bool DefineTexture(const std::string& name, GE::Texture* init_t) override;
		virtual void UnDefineTexture(const std::string& name) override;
		virtual const std::vector<std::pair<std::string, GE::Texture*>>& GetBindedTextures() const override;

		// 추후 텍스쳐 선언하는것도 만들자
		virtual GE::MaterialInstance* CreateMaterialInstance() override;
		virtual void DeleteMaterialInstance(GE::MaterialInstance* instance) override;
		virtual uint32_t GetMaterialInstanceCount() const override;


		virtual bool IsDirty() const override;
		virtual void ClearDirty() override;
		virtual void SendDirty() override;


		virtual const std::string& GetCode() const override;
		virtual const std::string& GetMaterialCBCode() const override;
		virtual std::vector<std::string> GetMaterialDataNameList() const override;
		virtual std::vector<std::string> GetMaterialTextureNameList() const override;
		virtual const std::string& GetErrorCode() const override {
			return m_ErrorCode;
		}
		virtual ~DX12_Material() override {}
	private:
		template<typename T, ShaderDataType DataType>
		bool DefineVarAs(const std::string& name, const T& data)
		{
			SendDirty();
			auto iter = std::find_if(m_DataBlocks.begin(), m_DataBlocks.end(), [&](const DX12_MaterialInstance::DataBlock& block) -> bool
			{
				return block.name == name;
			});

			if (iter != m_DataBlocks.end()) {

				m_ErrorCode = "DX12_Material::DefineVarAs : Var Duplicate declared";
				return false;
			}

			DX12_MaterialInstance::DataBlock block;
			block.name = name;
			block.type = DataType;
			block.pos = m_BtPos;
			block.size = GetShaderDataTypeByteSize(block.type);
			if (block.size == 0) {
				m_ErrorCode = "DX12_Material::DefineVarAs : BlockSize is 0";
				return false;
			}

			m_BtPos += block.size;

			// 데이터 복사
			size_t btSize = m_BtData.size();
			if (btSize < m_BtPos)
			{
				btSize += SHADER_CONSTANT_BUFFER_UNIT_SIZE;
				m_BtData.resize(btSize);
			}
			m_DataBlocks.push_back(block);
			memcpy(&m_BtData[block.pos], &data, block.size);

			m_CBCode += ToString(block.type) + " " + name + SHADER_NAME_SUFFIX_MATERIAL_DATA + "; \n";


			//
			for (auto& ins : m_MaterialInstances)
			{
				DX12_MaterialInstance* ptr = (DX12_MaterialInstance * )ins.get();
				
				ptr->m_BtData.resize(btSize);
				ptr->m_DataBlocks.push_back(block);
				memcpy(&ptr->m_BtData[block.pos], &data, block.size);
			}

			return true;
		}


	private:
		bool m_IsDirty = true;
		mutable std::string m_ErrorCode;

		//

		std::string m_ShaderCode;
		mutable std::string m_CBCode;
		GE::MaterialProperty m_Property;
		//

		using BindedTextureArray = std::vector<std::pair<std::string, GE::Texture*>>;
		BindedTextureArray m_BindedTextures;



		// Material CB Data
		std::vector<DX12_MaterialInstance::DataBlock> m_DataBlocks;
		std::vector<char>      m_BtData;
		uint32_t               m_BtPos = 0;
		//

		//
		std::vector<std::unique_ptr<GE::MaterialInstance>> m_MaterialInstances;

	};
}