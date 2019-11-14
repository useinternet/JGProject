#pragma once

#include "Object/ReObject.h"
#include "ShaderDefined.h"
#include "ShaderDataType.h"
#include <set>

namespace RE
{
	class Texture;
	class ShaderBindingData : public ReObject
	{
	public:
		ShaderBindingData();
		ShaderBindingData(const std::string& name);
	public:
		virtual std::string GetCode(uint32_t register_number, uint32_t register_space) = 0;
		JGShader::EShaderBindData GetType() const {
			return m_Type;
		}
		JGShader::EShaderBindDataState GetState() const {
			return m_State;
		}

		virtual void ConvertReadWrite() {
			m_State = JGShader::ReadWrite;
		}
		virtual void ConvertReadOnly() {
			m_State = JGShader::ReadOnly;
		}
		virtual void Save(std::ofstream& fout);
		virtual void Load(std::ifstream& fin);
	protected:
		JGShader::EShaderBindData m_Type;
		JGShader::EShaderBindDataState m_State = JGShader::ReadOnly;
	};

	class SBDConstantBuffer : public ShaderBindingData
	{
	public:
		SBDConstantBuffer();
		SBDConstantBuffer(const std::string& name);
	public:
		ShaderDataType* Add(const JGShader::EShaderData type, const std::string& var_name);
		ShaderDataType* Add(const std::string& struct_type_name, const std::string& var_name);
		void Remove(const std::string& var_name);
		ShaderDataType* Get(const std::string& var_name);

		std::vector<byte> GetData();
		uint32_t GetSize() const;

		virtual std::string GetCode(uint32_t register_number, uint32_t register_space) override;
		virtual void Save(std::ofstream& fout) override;
		virtual void Load(std::ifstream& fin) override;
	private:
		std::vector<std::string> m_DataVarNames;
		std::map<std::string, std::shared_ptr<ShaderDataType>> m_Datas;
	};

	class SBDStructuredBuffer : public ShaderBindingData
	{
	public:
		SBDStructuredBuffer();
		SBDStructuredBuffer(const std::string& name);
	public:
		void      BindStruct(const std::string& struct_type_name);
		STStruct* Add();
		void      Remove(STStruct* _struct);
		void      Remove(uint32_t idx);
		STStruct* Get(uint32_t idx);
		uint32_t  GetElementCount() const;
		void      Clear();
		std::vector<byte> GetData();
		uint32_t GetSize() const;
		virtual std::string GetCode(uint32_t register_number, uint32_t register_space) override;
		virtual void Save(std::ofstream& fout) override;
		virtual void Load(std::ifstream& fin) override;
	private:
		std::map<STStruct*, uint32_t>          m_ElementsIndex;
		std::vector<std::shared_ptr<STStruct>> m_Elements;
		std::shared_ptr<STStruct>              m_BindedStructType;
	};


	class SBDTexture2D : public ShaderBindingData
	{
	public:
		SBDTexture2D();
		SBDTexture2D(const std::string& name);
	public:
		virtual void     Add(const Texture& texture);
		virtual void     Set(uint32_t idx, const Texture& texture);
		Texture* Get(uint32_t idx);
		void     Remove(uint32_t idx);
		void     Clear();
		uint32_t Count() const;
		const std::vector<Texture>& GetData() const;
		virtual  std::string GetCode(uint32_t register_number, uint32_t register_space) override;
		virtual void Save(std::ofstream& fout) override;
		virtual void Load(std::ifstream& fin) override;

		virtual void ConvertReadWrite() override;
		virtual void ConvertReadOnly()  override;
	private:
		std::vector<Texture> m_Textures;
		
	};
	class SBDTextureCube : public ShaderBindingData
	{
	public:
		SBDTextureCube();
		SBDTextureCube(const std::string& name);

		void     Add(const Texture& texture);
		void     Set(uint32_t idx, const Texture& texture);
		Texture* Get(uint32_t idx);
		void     Remove(uint32_t idx);
		void     Clear();
		uint32_t Count() const;
		const std::vector<Texture>& GetData() const;
		virtual void ConvertReadWrite() override;
		virtual  std::string GetCode(uint32_t register_number, uint32_t register_space) override;
		virtual void Save(std::ofstream& fout) override;
		virtual void Load(std::ifstream& fin) override;
	private:
		std::vector<Texture> m_Textures;
	};


	inline std::shared_ptr<ShaderBindingData> GetSBDByType(JGShader::EShaderBindData type, const std::string& name = "")
	{
		std::shared_ptr<ShaderBindingData> result;
		switch (type)
		{
		case JGShader::EShaderBindData::ConstantBuffer:
			result = std::make_shared<SBDConstantBuffer>(name);
			break;
		case JGShader::EShaderBindData::StructuredBuffer:
			result = std::make_shared<SBDStructuredBuffer>(name);
			break;
		case JGShader::EShaderBindData::Texture2D:
			result = std::make_shared<SBDTexture2D>(name);
			break;
		case JGShader::EShaderBindData::TextureCube:
			result = std::make_shared<SBDTextureCube>(name);
			break;
		}
		return result;
	}
}