#include "pch.h"
#include "ShaderData.h"
#include "Object/DxObject/Resource.h"
#include "ShaderLib.h"
using namespace std;

namespace RE
{
	ShaderBindingData::ShaderBindingData() : ReObject("ShaderData")
	{

	}
	ShaderBindingData::ShaderBindingData(const std::string& name) : ReObject(name)
	{

	}

	void ShaderBindingData::Save(std::ofstream& fout)
	{
		// 이름
		DataIO::write(fout, GetName());
		// 타입
		DataIO::write(fout, m_Type);
		// 상태
		DataIO::write(fout, m_State);
		DataIO::write(fout, m_RegisterNumber);
		DataIO::write(fout, m_RegisterSpace);
	}
	void ShaderBindingData::Load(std::ifstream& fin)
	{
		string name;
		DataIO::read(fin, name); SetName(name);
		DataIO::read(fin, m_Type);
		DataIO::read(fin, m_State);
		DataIO::read(fin, m_RegisterNumber);
		DataIO::read(fin, m_RegisterSpace);
	}



	SBDConstantBuffer::SBDConstantBuffer() : ShaderBindingData("SBDConstantBuffer")
	{
		m_Type = JGShader::ConstantBuffer;
	}
	SBDConstantBuffer::SBDConstantBuffer(const std::string& name) : ShaderBindingData(name)
	{
		m_Type = JGShader::ConstantBuffer;
	}
	

	ShaderDataType* SBDConstantBuffer::Add(const JGShader::EShaderData type, const std::string& var_name)
	{
		if (m_Datas.find(var_name) != m_Datas.end())
			return nullptr;
		auto s_p = GetShaderDataTypeByType(type);

		auto p = s_p.get();

		m_DataVarNames.push_back(var_name);
		m_Datas[var_name] = s_p;

		return p;
	}
	ShaderDataType* SBDConstantBuffer::Add(const std::string& struct_type_name, const std::string& var_name)
	{
		if (m_Datas.find(var_name) != m_Datas.end())
			return nullptr;

		auto clone = GetShaderLibManager()->GetTypeLib()->CloneStruct(struct_type_name);
		if (clone == nullptr)
		{
			RE_LOG_ERROR("failed Add struct({0}) in {1}", struct_type_name, GetName());
			return nullptr;
		}
		auto p = clone.get();

		m_DataVarNames.push_back(var_name);
		m_Datas[var_name] = clone;
		return p;
	}
	void SBDConstantBuffer::Remove(const std::string& var_name)
	{
		if (m_Datas.find(var_name) == m_Datas.end())
			return;


		for (auto iter = m_DataVarNames.begin(); iter < m_DataVarNames.end();)
		{
			if (*iter == var_name)
				iter = m_DataVarNames.erase(iter);
			else
				++iter;
		}
		m_Datas.erase(var_name);
	}
	ShaderDataType* SBDConstantBuffer::Get(const std::string& var_name)
	{
		if (m_Datas.find(var_name) == m_Datas.end())
			return nullptr;
		return m_Datas[var_name].get();
	}

	std::vector<byte> SBDConstantBuffer::GetData()
	{
		std::vector<byte> result;
		for (auto& name : m_DataVarNames)
		{
			auto btData = m_Datas[name]->GetByteData();
			result.insert(result.end(), btData.begin(), btData.end());
		}
		return move(result);
	}
	uint32_t SBDConstantBuffer::GetSize() const
	{
		uint32_t size = 0;
		for (auto& data_pair : m_Datas)
		{
			size += data_pair.second->GetSize();
		}
		return size;
	}
	void SBDConstantBuffer::Save(std::ofstream& fout)
	{
		ShaderBindingData::Save(fout);
		// element 사이즈
		DataIO::write(fout, m_DataVarNames.size());
		// 
		for (auto& name : m_DataVarNames)
		{
			auto element = m_Datas[name];
			// 타입, 타입이름
			DataIO::write(fout, element->GetType());
			if (element->GetType() == JGShader::_struct)
			{
				DataIO::write(fout, element->GetName());
			}
	

			// var 이름
			DataIO::write(fout, name);
		}
	}
	void SBDConstantBuffer::Load(std::ifstream& fin)
	{
		ShaderBindingData::Load(fin);
		size_t size;
		DataIO::read(fin, size);
		for (size_t i = 0; i < size; ++i)
		{
			shared_ptr<ShaderDataType> data;
			JGShader::EShaderData type;
			DataIO::read(fin, type);
			if (type == JGShader::_struct)
			{
				string struct_name;
				DataIO::read(fin, struct_name);
				data = GetShaderLibManager()->GetTypeLib()->CloneStruct(struct_name);
				if (data == nullptr)
				{
					RE_LOG_ERROR("failed SBDConstantBuffer : {0} Load -> find struct in ShaderTypeLib.jg", struct_name);
				}
			}
			else
			{
				data = GetShaderDataTypeByType(type);
			}
			string var_name;
			DataIO::read(fin, var_name);
			m_DataVarNames.push_back(var_name);
			m_Datas[var_name] = data;
		}
	}
	std::string SBDConstantBuffer::GetCode(uint32_t register_number, uint32_t register_space)
	{
		std::string code = "cbuffer " + GetName() + " : register(b" + to_string(register_number) + ", space" +
			to_string(register_space) + ") \n{\n";

		for (auto& name : m_DataVarNames)
		{
			auto st = m_Datas[name];
			code += "   " + st->GetName() + " " + name + ";\n";
		}

		code += "};\n";
		return code;
	}



	SBDStructuredBuffer::SBDStructuredBuffer() :
		ShaderBindingData("SBDStructuredBuffer")
	{
		m_Type = JGShader::StructuredBuffer;
	}
	SBDStructuredBuffer::SBDStructuredBuffer(const std::string& name) :
		ShaderBindingData(name)
	{
		m_Type = JGShader::StructuredBuffer;
	}

	void      SBDStructuredBuffer::BindStruct(const std::string& struct_type_name)
	{
		if (m_BindedStructType)
			Clear();

		m_BindedStructType = GetShaderLibManager()->GetTypeLib()->CloneStruct(struct_type_name);
		if (m_BindedStructType == nullptr)
		{
			RE_LOG_ERROR("failed bindStruct({0}) in {1}", struct_type_name, GetName());
		}
	}
	STStruct* SBDStructuredBuffer::Add()
	{
		if (m_BindedStructType == nullptr)
			return nullptr;
		shared_ptr<STStruct> element = make_shared<STStruct>(*m_BindedStructType);
		STStruct* result = element.get();

		uint32_t count = (uint32_t)m_Elements.size();
		m_Elements.push_back(move(element));
		m_ElementsIndex[result] = count;

		return result;
	}
	STStruct* SBDStructuredBuffer::Add(const STStruct& _struct)
	{
		auto s = Add();
		*s = _struct;
		return s;
	}
	void      SBDStructuredBuffer::Remove(STStruct* _struct)
	{
		if (m_ElementsIndex.find(_struct) == m_ElementsIndex.end())
			return;
		
		uint32_t idx = m_ElementsIndex[_struct];
		auto iter = m_Elements.begin() + idx;


		m_ElementsIndex.erase(_struct);
		m_Elements.erase(iter);
	}
	void      SBDStructuredBuffer::Remove(uint32_t idx)
	{
		if (m_Elements.size() <= idx)
			return;


		auto p = m_Elements[idx];
		m_ElementsIndex.erase(p.get());
		auto iter = m_Elements.begin() + idx;
		m_Elements.erase(iter);
	}
	STStruct* SBDStructuredBuffer::Get(uint32_t idx)
	{
		if (m_Elements.size() <= idx)
			return nullptr;
		return m_Elements[idx].get();
	}
	uint32_t  SBDStructuredBuffer::GetElementCount() const
	{
		return (uint32_t)m_Elements.size();
	}
	std::vector<byte> SBDStructuredBuffer::GetData()
	{
		std::vector<byte> bt;
		for (auto& element : m_Elements)
		{
			auto src = element->GetByteData();
			bt.insert(bt.end(), src.begin(), src.end());
		}
		return move(bt);
	}
	uint32_t SBDStructuredBuffer::GetSize() const
	{
		if (m_BindedStructType == nullptr)
			return 0;
		uint32_t size = m_BindedStructType->GetSize();
		return GetElementCount() * size;
	}
	void SBDStructuredBuffer::Clear()
	{
		m_ElementsIndex.clear();
		m_Elements.clear();
		//m_BindedStructType.reset();
	}
	std::string SBDStructuredBuffer::GetCode(uint32_t register_number, uint32_t register_space)
	{
		std::string code;
		switch (m_State)
		{
		case JGShader::ReadOnly:
			code += "StructuredBuffer<" + m_BindedStructType->GetName() + "> " + GetName() +
				" : register(t" + to_string(register_number) + ", space" + to_string(register_space) + ");\n";
			break;
		case JGShader::ReadWrite:
			code += "RWStructuredBuffer<" + m_BindedStructType->GetName() + "> " + GetName() +
				" : register(u" + to_string(register_number) + ", space" + to_string(register_space) + ");\n";
			break;
		}
		return code;
	}

	void SBDStructuredBuffer::Save(std::ofstream& fout)
	{
		ShaderBindingData::Save(fout);
		DataIO::write(fout, m_BindedStructType->GetName());
	}

	void SBDStructuredBuffer::Load(std::ifstream& fin)
	{
		ShaderBindingData::Load(fin);
		string struct_name;
		DataIO::read(fin, struct_name);
		BindStruct(struct_name);
	}


	SBDTexture2D::SBDTexture2D() : ShaderBindingData("SBDTexture2D")
	{
		m_Type = JGShader::Texture2D;
	}
	SBDTexture2D::SBDTexture2D(const std::string& name) :
		ShaderBindingData(name)
	{
		m_Type = JGShader::Texture2D;
	}

	void     SBDTexture2D::Add(const Texture& texture)
	{
		Texture t = texture;
		auto desc = t.GetDesc();
		
		switch (m_State)
		{
		case JGShader::ReadWrite:
			if (!(desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS))
			{
				desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
				t.SetDesc(desc);
			}
			break;
		case JGShader::ReadOnly:
			if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
			{
				desc.Flags &= (~D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
				t.SetDesc(desc);
			}
	
			break;
		}

		m_Textures.push_back(move(t));
	}
    void  SBDTexture2D::Resize(uint32_t count)
	{
		m_Textures.resize(count);
	}
	Texture* SBDTexture2D::Get(uint32_t idx)
	{
		if (m_Textures.size() <= idx)
			return nullptr;

		return &m_Textures[idx];
	}
	void     SBDTexture2D::Set(uint32_t idx, const Texture& texture)
	{
		if (m_Textures.size() <= idx)
			return;

		Texture t = texture;
		auto desc = t.GetDesc();

		switch (m_State)
		{
		case JGShader::ReadWrite:
			if (!(desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS))
			{
				desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
				t.SetDesc(desc);
			}
			break;
		case JGShader::ReadOnly:
			if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
			{
				desc.Flags &= (~D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
				t.SetDesc(desc);
			}
			break;
		}
		m_Textures[idx] = move(t);
	}
	void     SBDTexture2D::Remove(uint32_t idx)
	{
		if (m_Textures.size() <= idx)
			return;
		auto iter = m_Textures.begin() + idx;
		m_Textures.erase(iter);
	}
	void     SBDTexture2D::Clear()
	{
		m_Textures.clear();
	}
	uint32_t SBDTexture2D::Count() const
	{
		return (uint32_t)m_Textures.size();
	}
	const std::vector<Texture>& SBDTexture2D::GetData() const
	{
		return m_Textures;
	}
	std::string SBDTexture2D::GetCode(uint32_t register_number, uint32_t register_space)
	{
		std::string code;
		switch (m_State)
		{
		case JGShader::ReadOnly:
			if (m_Textures.empty())
			{

			}
			else if (m_Textures.size() == 1)
			{
				code += "Texture2D " + GetName() + " : register(t" + to_string(register_number) + ", space" +
					to_string(register_space) + ");\n";
			}
			else
			{
				code += "Texture2D " + GetName() + "[" + to_string(Count()) +  "] : register(t" + 
					to_string(register_number) + ", space" + to_string(register_space) + ");\n";
			}

			break;
		case JGShader::ReadWrite:
			if (m_Textures.empty())
			{

			}
			else if (m_Textures.size() == 1)
			{
				code += "RWTexture2D " + GetName() + " : register(u" + to_string(register_number) + ", space" +
					to_string(register_space) + ");\n";
			}
			else
			{
				code += "RWTexture2D " + GetName() + "[" + to_string(Count()) + "] : register(u" +
					to_string(register_number) + ", space" + to_string(register_space) + ");\n";
			}
			break;
		}
		return code;
	}
	void SBDTexture2D::Save(std::ofstream& fout)
	{
		ShaderBindingData::Save(fout);
		DataIO::write(fout, m_Textures.size());
	}
	void SBDTexture2D::Load(std::ifstream& fin)
	{
		ShaderBindingData::Load(fin);
		size_t size;
		DataIO::read(fin, size);
		m_Textures.resize(size);
	}
	void SBDTexture2D::ConvertReadWrite()
	{
		ShaderBindingData::ConvertReadWrite();
		for (auto& t : m_Textures)
		{
			auto desc = t.GetDesc();
			if (!(desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS))
			{
				desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
				t.SetDesc(desc);
			}
		}
	}
	void SBDTexture2D::ConvertReadOnly()
	{
		ShaderBindingData::ConvertReadOnly();
		for (auto& t : m_Textures)
		{
			auto desc = t.GetDesc();
			if (desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
			{
				desc.Flags &= (~D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
				t.SetDesc(desc);
			}
		}
	}

	SBDTextureCube::SBDTextureCube() : ShaderBindingData("SBDTextureCube")
	{
		m_Type = JGShader::TextureCube;
	}
	SBDTextureCube::SBDTextureCube(const std::string& name) : ShaderBindingData(name)
	{
		m_Type = JGShader::TextureCube;
	}

	void     SBDTextureCube::Add(const Texture& texture)
	{
		Texture t = texture;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = t.GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		srvDesc.Format = t.GetDesc().Format;
		t.SetSrvDesc(srvDesc);



		m_Textures.push_back(move(t));
	}
	void     SBDTextureCube::Set(uint32_t idx, const Texture& texture)
	{
		if (m_Textures.size() <= idx)
			return;
		Texture t = texture;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = t.GetDesc().MipLevels;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
		srvDesc.Format = t.GetDesc().Format;
		t.SetSrvDesc(srvDesc);
		m_Textures[idx] = move(t);

	}
	Texture* SBDTextureCube::Get(uint32_t idx)
	{
		if (m_Textures.size() <= idx)
			return nullptr;
		return &m_Textures[idx];
	}
	void     SBDTextureCube::Remove(uint32_t idx)
	{
		if (m_Textures.size() <= idx)
			return;

		auto iter = m_Textures.begin() + idx;
		m_Textures.erase(iter);

	}
	void     SBDTextureCube::Clear()
	{
		m_Textures.clear();
	}
	uint32_t SBDTextureCube::Count() const
	{
		return (uint32_t)m_Textures.size();
	}

	const std::vector<Texture>& SBDTextureCube::GetData() const
	{
		return m_Textures;
	}
	void SBDTextureCube::ConvertReadWrite()
	{
		m_State = JGShader::ReadOnly;
	}
	std::string SBDTextureCube::GetCode(uint32_t register_number, uint32_t register_space)
	{
		std::string code = "TextureCube " + GetName();
		if (m_Textures.empty())
			return "";
		if(m_Textures.size() > 1)
		{ 
			code += "[" + to_string(Count()) + "]";
		}
		code += " : register(t" + to_string(register_number) + ", space" + to_string(register_space) + ");\n";
		return code;
	}
	void SBDTextureCube::Save(std::ofstream& fout)
	{
		ShaderBindingData::Save(fout);
		DataIO::write(fout, m_Textures.size());
	}
	void SBDTextureCube::Load(std::ifstream& fin)
	{
		ShaderBindingData::Load(fin);
		size_t size;
		DataIO::read(fin, size);
		m_Textures.resize(size);
	}
}