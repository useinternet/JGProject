#include "pch.h"
#include "ShaderCode.h"
#include "ShaderModule.h"
#include "ShaderRegisterMap.h"
using namespace std;
namespace RE
{
	ShaderCode::ShaderCode(const std::string& name, ShaderEnum::EShaderVisible visible) :
		 m_Name(name), m_Visible(visible)  { }
	// Shader 구조체
	void SCStruct::Set(const std::string& struct_name, ShaderEnum::EShaderVisible visible) {
		CodeSet(struct_name, visible);
	}
	void SCStruct::AddVar(const SCVar& data)
	{
		m_Size += data.GetSize();
		m_Datas.push_back(std::make_shared<SCVar>(data));
		m_DataNames.push_back(data.GetName());
	}
	void SCStruct::AddVar(
		ShaderEnum::EShaderDataType type,
		const std::string& name,
		uint32_t array_size, 
		ShaderEnum::EShaderVisible visible)
	{
		SCVar var(type, name, array_size, visible);
		AddVar(var);
	}
	void SCStruct::AddVar(const SCStruct& _struct, const std::string& var_name)
	{
		m_Size += _struct.GetSize();
		m_Datas.push_back(std::make_shared<SCStruct>(_struct));
		m_DataNames.push_back(var_name);
		// 구현중
	}
	uint32_t SCStruct::GetSize() const
	{
		return m_Size;
	}
	std::string SCStruct::GetShaderCode() const
	{
		std::string code = "struct " + GetName() + "\n{\n";

		for (auto& data : m_Datas)
		{
			code += "   " + data->GetShaderCode();
		}
		code += "};\n";
		return code;
	}

	//
	void SCInputStruct::Set(const std::string& struct_name, ShaderEnum::EShaderVisible visible)
	{
		CodeSet(struct_name, visible);
	}
	void SCInputStruct::AddVar(const SCVar& var, const std::string& semantic_name)
	{
		m_Datas.push_back(make_shared<SCVar>(var));
		m_SemanticNames.push_back(semantic_name);
	}
	void SCInputStruct::AddVar(const ShaderEnum::EShaderDataType type, const std::string& var_name, const std::string& semantic_name)
	{
		SCVar var(type, var_name, 1, GetVisible());
		m_Datas.push_back(make_shared<SCVar>(var));
		m_SemanticNames.push_back(semantic_name);
	}
	std::string SCInputStruct::GetShaderCode() const
	{
		std::string code = "struct " + GetName() + "\n{\n";
		uint32_t count = (uint32_t)m_Datas.size();

		for (uint32_t i = 0; i < count; ++i)
		{
			auto typeStr = ShaderEnum::ShaderDataTypeToString(m_Datas[i]->GetType());
			code += "   " + typeStr + " " + m_Datas[i]->GetName() + " : " + m_SemanticNames[i] + ";\n";
		}
		code += "};\n";
		return code;
	}
	std::vector<D3D12_INPUT_ELEMENT_DESC> SCInputStruct::GetInputElementDescArray() const
	{
		uint32_t count = (uint32_t)m_Datas.size();
		std::vector<D3D12_INPUT_ELEMENT_DESC> result(count);

		uint32_t strid = 0;
		for (uint32_t i = 0; i < count; ++i)
		{

			result[i] = {
				m_SemanticNames[i].c_str(), 0, 
				ShaderEnum::ShaderDataTypeToFormat(m_Datas[i]->GetType()), 0,
				strid, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0};

			strid += (uint32_t)ShaderEnum::ShaderDataTypeToSize(m_Datas[i]->GetType());
		}

		return result;
	}


	//
	void SCVar::Set(ShaderEnum::EShaderDataType type, const std::string& name, uint32_t array_size, ShaderEnum::EShaderVisible visible)
	{
		ShaderCode::CodeSet(name, visible);
		m_Type = type;
		m_ArraySize = array_size;
	}
	ShaderEnum::EShaderDataType SCVar::GetType() const
	{
		return m_Type;
	}
	uint32_t SCVar::GetArraySize() const
	{
		return m_ArraySize;
	}
	uint32_t SCVar::GetSize() const
	{
		return (uint32_t)ShaderEnum::ShaderDataTypeToSize(m_Type) * m_ArraySize;
	}
	std::string SCVar::GetShaderCode() const  {

		std::string code = ShaderEnum::ShaderDataTypeToString(m_Type) + " " + GetName();
		if (m_ArraySize <= 1)
		{
			code += ";\n";
		}
		else
		{
			code += "[" + std::to_string(m_ArraySize) + "];\n";
		}
		return code;
	}
	void SDResource::Set(
		ShaderEnum::EShaderResourceType type,
		const std::string& name,
		uint32_t init_array_size,
		ShaderEnum::EShaderVisible visible)
	{
		if (m_OwnerModule->FindBindedSDResource(GetName()))
		{
			auto registermap = m_OwnerModule->GetRegisterMap();
			switch (type)
			{
			case ShaderEnum::Texture2D:
			case ShaderEnum::TextureCube:
				registermap->t2DDeRegister(GetName());
				break;
			case ShaderEnum::RWTexture2D:
			case ShaderEnum::RWTextureCube:
				registermap->tCubeDeRegister(GetName());
				break;
			}
		}


		m_BindedResources.clear();
		SetName(name);
		SetVisible(visible); m_ResourceType = type;
		m_BindedResources.resize(init_array_size);


		if (m_OwnerModule->FindBindedSDResource(GetName()))
		{
			auto registermap = m_OwnerModule->GetRegisterMap();
			switch (type)
			{
			case ShaderEnum::Texture2D:
			case ShaderEnum::TextureCube:
				registermap->t2DRegister(this);
				break;
			case ShaderEnum::RWTexture2D:
			case ShaderEnum::RWTextureCube:
				registermap->tCubeRegister(this);
				break;
			}
		}
	}



	void SDResource::BindResource(const Texture& resource)
	{
		if (m_OwnerModule->FindBindedSDResource(GetName()))
		{
			auto registermap = m_OwnerModule->GetRegisterMap();
			switch (GetResourceType())
			{
			case ShaderEnum::Texture2D:
			case ShaderEnum::TextureCube:
				registermap->t2DDeRegister(GetName());
				break;
			case ShaderEnum::RWTexture2D:
			case ShaderEnum::RWTextureCube:
				registermap->tCubeDeRegister(GetName());
				break;
			}
		}
		m_BindedResources.clear();
		m_BindedResources.push_back(resource);
		if (m_OwnerModule->FindBindedSDResource(GetName()))
		{
			auto registermap = m_OwnerModule->GetRegisterMap();
			switch (GetResourceType())
			{
			case ShaderEnum::Texture2D:
			case ShaderEnum::TextureCube:
				registermap->t2DRegister(this);
				break;
			case ShaderEnum::RWTexture2D:
			case ShaderEnum::RWTextureCube:
				registermap->tCubeRegister(this);
				break;
			}
		}
	}

	void SDResource::BindResource(const std::vector<Texture>& resource)
	{
		if (m_OwnerModule->FindBindedSDResource(GetName()))
		{
			auto registermap = m_OwnerModule->GetRegisterMap();
			switch (GetResourceType())
			{
			case ShaderEnum::Texture2D:
			case ShaderEnum::TextureCube:
				registermap->t2DDeRegister(GetName());
				break;
			case ShaderEnum::RWTexture2D:
			case ShaderEnum::RWTextureCube:
				registermap->tCubeDeRegister(GetName());
				break;
			}
		}
		m_BindedResources.clear();
		m_BindedResources = move(resource);
		if (m_OwnerModule->FindBindedSDResource(GetName()))
		{
			auto registermap = m_OwnerModule->GetRegisterMap();
			switch (GetResourceType())
			{
			case ShaderEnum::Texture2D:
			case ShaderEnum::TextureCube:
				registermap->t2DRegister(this);
				break;
			case ShaderEnum::RWTexture2D:
			case ShaderEnum::RWTextureCube:
				registermap->tCubeRegister(this);
				break;
			}
		}
	}

	void SDResource::SetResourceType(ShaderEnum::EShaderResourceType type)
	{
		m_ResourceType = type;
	}

	uint32_t SDResource::GetBindedResourceSize() const
	{
		return (uint32_t)m_BindedResources.size();
	}

	const std::vector<Texture> SDResource::GetBindedResources() const
	{
		return m_BindedResources;
	}


	ShaderEnum::EShaderResourceType SDResource::GetResourceType() const
	{
		return m_ResourceType;
	}

	std::string SDResource::GetShaderCode() const
	{
		string code = ShaderEnum::ShaderResourceTypeToString(m_ResourceType) + " " + GetName();


		if (m_BindedResources.size() > 1)
		{
			code += "[" + to_string(m_BindedResources.size()) + "]";
		}
		code += " : register(t" + to_string(GetRegisterNumber()) + ", space" +
			to_string(GetRegisterSpace()) + "); \n";

		return code;
	}


	void SDSamplerState::Set(
		const D3D12_STATIC_SAMPLER_DESC& desc,
		const std::string& name,
		ShaderEnum::EShaderVisible visible)
	{
		SetName(name); 
		SetVisible(visible);
		m_Desc = make_shared<D3D12_STATIC_SAMPLER_DESC>(desc);
	}
	void SDSamplerState::SetDesc(const D3D12_STATIC_SAMPLER_DESC& desc)
	{
		*m_Desc = desc;
		SetRegisterIndex(desc.ShaderRegister, desc.RegisterSpace);
	}
	const D3D12_STATIC_SAMPLER_DESC& SDSamplerState::GetDesc() const
	{
		return *m_Desc;
	}
	void SDSamplerState::SetRegisterIndex(uint32_t register_number, uint32_t register_space)
	{
		ShaderData::SetRegisterIndex(register_number, register_space);
		m_Desc->ShaderRegister = register_number;
		m_Desc->RegisterSpace = register_space;
	}
	std::string SDSamplerState::GetShaderCode() const
	{
		string code = "SamplerState " + GetName() + " : register(s" +
			to_string(GetRegisterNumber()) + ", space" + to_string(GetRegisterSpace()) + ");\n";
		return code;
	}
	void SDConstantBuffer::Set(
		const std::string& name,
		ShaderEnum::EShaderVisible visible)
	{
		SetName(name); 
		SetVisible(visible);
	}
	void SDConstantBuffer::AddVar(const SCVar& var)
	{
		auto iter = m_DataSheet.find(var.GetName());
		if (iter != m_DataSheet.end())
		{
			RE_LOG_ERROR("{0} has been redefined. in {1}", var.GetName(), GetName());
			return;
		}


		m_DataSheet[var.GetName()].resize(var.GetSize());
		m_SCVars.push_back(std::make_shared<SCVar>(var));
	}
	void SDConstantBuffer::AddVar(
		ShaderEnum::EShaderDataType type, 
		const std::string& name, 
		uint32_t array_size,
		ShaderEnum::EShaderVisible visible)
	{
		SCVar var;
		var.Set(type, name, array_size, visible);
		AddVar(var);
	}
	void SDConstantBuffer::AddVar(const SCStruct& var)
	{
		auto iter = m_DataSheet.find(var.GetName());
		if (iter != m_DataSheet.end())
		{
			RE_LOG_ERROR("{0} has been redefined. in {1}", var.GetName(), GetName());
			return;
		}




		m_DataSheet[var.GetName()].resize(var.GetSize());
		m_SCVars.push_back(std::make_shared<SCStruct>(var));
	}

	uint32_t SDConstantBuffer::GetDataSize() const
	{
		uint32_t size = 0;
		for (auto& data : m_DataSheet)
		{
			size += (uint32_t)data.second.size();
		}
		return size;
	}

	std::vector<byte> SDConstantBuffer::GetData() const
	{
		std::vector<byte> result;
		uint32_t var_count = (uint32_t)m_SCVars.size();


		for (auto& var : m_SCVars)
		{
			auto iter = m_DataSheet.find(var->GetName());
			result.insert(result.end(), iter->second.begin(), iter->second.end());
		}
		return result;
	}

	std::string SDConstantBuffer::GetShaderCode() const
	{
		std::string code = "cbuffer " + GetName() + " : ";
		code += "register(b" + std::to_string(GetRegisterNumber()) +
			", space" + std::to_string(GetRegisterSpace()) + ")\n{\n";


		for (auto& var : m_SCVars)
		{
			code += "   " + var->GetShaderCode();
		}

		code += "};\n";

		return code;
	}

	void SDStructuredBuffer::Set(
		const std::string& define_name, 
		const SCStruct& _struct, 
		ShaderEnum::EShaderVisible visible)
	{
		SetName(define_name);
		SetVisible(visible);
		m_StructDataType = make_shared<SCStruct>(_struct);

	}
	std::vector<byte> SDStructuredBuffer::GetData() const
	{
		return m_Data;
	}
	std::string SDStructuredBuffer::GetShaderCode() const
	{
		string code = "StructuredBuffer<" + m_StructDataType->GetName() + "> ";
		code += 
			GetName() + " : " + "register(t" + to_string(GetRegisterNumber()) + 
			", space" + to_string(GetRegisterSpace());
		code += ");\n";
		return code;
	}

}
