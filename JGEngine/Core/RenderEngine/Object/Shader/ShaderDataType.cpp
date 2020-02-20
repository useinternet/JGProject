#include "pch.h"
#include "ShaderDataType.h"
#include "ShaderLib.h"
using namespace std;

namespace RE
{
	void ShaderDataType::Save(std::ofstream& fout)
	{
		DataIO::write(fout, GetName());
		DataIO::write(fout, GetType());
		DataIO::write(fout, GetSize());
		DataIO::write(fout, m_SortingOrder);
	}
	void ShaderDataType::Load(std::ifstream& fin)
	{
		std::string name;
		DataIO::read(fin, name); SetName(name);
		DataIO::read(fin, m_Type);
		DataIO::read(fin, m_Size);
		DataIO::read(fin, m_SortingOrder);
	}

	STStruct::STStruct(const STStruct& copy) : ShaderDataType(copy.GetName())
	{
		for (auto& n : copy.m_SortingOrderMap)
		{
			auto element = n.second.first;
			auto name = n.second.second;
			if (element->GetType() == JGShader::_struct)
			{
				STStruct* p = (STStruct*)element;
				Add(*p, name);
			}
			else
			{
				auto e = Add(element->GetType(), name);
				switch (e->GetType())
				{
				case JGShader::_int:
					CastShaderDataType<STInt>(e)->Set(CastShaderDataType<STInt>(element)->Get());
					break;
				case JGShader::_int2:
					CastShaderDataType<STInt2>(e)->Set(CastShaderDataType<STInt2>(element)->Get());
					break;
				case JGShader::_int3:
					CastShaderDataType<STInt3>(e)->Set(CastShaderDataType<STInt3>(element)->Get());
					break;
				case JGShader::_int4:
					CastShaderDataType<STInt4>(e)->Set(CastShaderDataType<STInt4>(element)->Get());
					break;
				case JGShader::_uint:
					CastShaderDataType<STUint>(e)->Set(CastShaderDataType<STUint>(element)->Get());
					break;
				case JGShader::_uint2:
					CastShaderDataType<STUint2>(e)->Set(CastShaderDataType<STUint2>(element)->Get());
					break;
				case JGShader::_uint3:
					CastShaderDataType<STUint3>(e)->Set(CastShaderDataType<STUint3>(element)->Get());
					break;
				case JGShader::_uint4:
					CastShaderDataType<STUint4>(e)->Set(CastShaderDataType<STUint4>(element)->Get());
					break;
				case JGShader::_float:
					CastShaderDataType<STFloat>(e)->Set(CastShaderDataType<STFloat>(element)->Get());
					break;
				case JGShader::_float2:
					CastShaderDataType<STFloat2>(e)->Set(CastShaderDataType<STFloat2>(element)->Get());
					break;
				case JGShader::_float3:
					CastShaderDataType<STFloat3>(e)->Set(CastShaderDataType<STFloat3>(element)->Get());
					break;
				case JGShader::_float4:
					CastShaderDataType<STFloat4>(e)->Set(CastShaderDataType<STFloat4>(element)->Get());
					break;
				case JGShader::_matrix4x4:
					CastShaderDataType<STMatrix>(e)->Set(CastShaderDataType<STMatrix>(element)->Get());
					break;
				}
			}
		}
	}
	STStruct& STStruct::operator=(const STStruct& copy)
	{
		m_Size = 0;
		m_SortingOrder = -1;
		m_StructElements.clear();
		m_SortingOrderMap.clear();
		m_SortingIndex = 0;
		for (auto& n : copy.m_SortingOrderMap)
		{
			auto element = n.second.first;
			auto name = n.second.second;
			if (element->GetType() == JGShader::_struct)
			{
				STStruct* p = (STStruct*)element;
				Add(*p, name);
			}
			else
			{
				auto e = Add(element->GetType(), name);
				switch (e->GetType())
				{
				case JGShader::_int:
					CastShaderDataType<STInt>(e)->Set(CastShaderDataType<STInt>(element)->Get());
					break;
				case JGShader::_int2:
					CastShaderDataType<STInt2>(e)->Set(CastShaderDataType<STInt2>(element)->Get());
					break;
				case JGShader::_int3:
					CastShaderDataType<STInt3>(e)->Set(CastShaderDataType<STInt3>(element)->Get());
					break;
				case JGShader::_int4:
					CastShaderDataType<STInt4>(e)->Set(CastShaderDataType<STInt4>(element)->Get());
					break;
				case JGShader::_uint:
					CastShaderDataType<STUint>(e)->Set(CastShaderDataType<STUint>(element)->Get());
					break;
				case JGShader::_uint2:
					CastShaderDataType<STUint2>(e)->Set(CastShaderDataType<STUint2>(element)->Get());
					break;
				case JGShader::_uint3:
					CastShaderDataType<STUint3>(e)->Set(CastShaderDataType<STUint3>(element)->Get());
					break;
				case JGShader::_uint4:
					CastShaderDataType<STUint4>(e)->Set(CastShaderDataType<STUint4>(element)->Get());
					break;
				case JGShader::_float:
					CastShaderDataType<STFloat>(e)->Set(CastShaderDataType<STFloat>(element)->Get());
					break;
				case JGShader::_float2:
					CastShaderDataType<STFloat2>(e)->Set(CastShaderDataType<STFloat2>(element)->Get());
					break;
				case JGShader::_float3:
					CastShaderDataType<STFloat3>(e)->Set(CastShaderDataType<STFloat3>(element)->Get());
					break;
				case JGShader::_float4:
					CastShaderDataType<STFloat4>(e)->Set(CastShaderDataType<STFloat4>(element)->Get());
					break;
				case JGShader::_matrix4x4:
					CastShaderDataType<STMatrix>(e)->Set(CastShaderDataType<STMatrix>(element)->Get());
					break;
				}
			}
		}
		return *this;
	}

	ShaderDataType* STStruct::Add(JGShader::EShaderData data_type, const std::string& name)
	{
		if (m_StructElements.find(name) != m_StructElements.end())
			return nullptr;
		std::shared_ptr<ShaderDataType> element;
		switch (data_type)
		{
		case JGShader::_int:
			element = make_shared<STInt>();
			break;
		case JGShader::_int2:
			element = make_shared<STInt2>();
			break;
		case JGShader::_int3:
			element = make_shared<STInt3>();
			break;
		case JGShader::_int4:
			element = make_shared<STInt4>();
			break;
		case JGShader::_uint:
			element = make_shared<STUint>();
			break;
		case JGShader::_uint2:
			element = make_shared<STUint2>();
			break;
		case JGShader::_uint3:
			element = make_shared<STUint3>();
			break;
		case JGShader::_uint4:
			element = make_shared<STUint4>();
			break;
		case JGShader::_float:
			element = make_shared<STFloat>();
			break;
		case JGShader::_float2:
			element = make_shared<STFloat2>();
			break;
		case JGShader::_float3:
			element = make_shared<STFloat3>();
			break;
		case JGShader::_float4:
			element = make_shared<STFloat4>();
			break;
		case JGShader::_matrix4x4:
			element = make_shared<STMatrix>();
			break;
		default:
			return nullptr;
		}


		element->m_SortingOrder = m_SortingIndex++;
		m_Size += element->m_Size;


		ShaderDataType* pt = element.get();
		m_StructElements[name] = move(element);
		m_SortingOrderMap[pt->m_SortingOrder] =  pair<ShaderDataType*, std::string>(pt, name);

		return pt;
	}
	ShaderDataType* STStruct::Add(const STStruct& _struct, const std::string& name)
	{
		if (m_StructElements.find(name) != m_StructElements.end())
			return nullptr;

		std::shared_ptr<ShaderDataType> element = make_shared<STStruct>(_struct);
		element->m_SortingOrder = m_SortingIndex++;
		m_Size += element->m_Size;


		ShaderDataType* pt = element.get();
		m_StructElements[name] = move(element);
		m_SortingOrderMap[element->m_SortingOrder] = pair<ShaderDataType*, std::string>(pt, name);

		return pt;
	}
	void STStruct::Remove(const std::string& var_name)
	{
		if (m_StructElements.find(var_name) == m_StructElements.end())
			return;

		uint32_t order = m_StructElements[var_name]->m_SortingOrder;
		uint32_t size = m_StructElements[var_name]->m_Size;
		uint32_t sortingorder = m_StructElements[var_name]->m_SortingOrder;

		m_SortingOrderMap.erase(sortingorder);
		
		// Sorting Order값 재조정
		for (uint32_t i = sortingorder + 1; i < m_SortingIndex; ++i)
		{
			auto value = m_SortingOrderMap[i];
			m_SortingOrderMap.erase(i);
			m_SortingOrderMap[i - 1] = value;
			value.first->m_SortingOrder = i - 1;
		}
		--m_SortingIndex;
		// 삭제
		m_StructElements.erase(var_name);
	}
	ShaderDataType* STStruct::GetElement(const std::string& var_name) 
	{
		if (m_StructElements.find(var_name) == m_StructElements.end())
			return nullptr;
		return m_StructElements[var_name].get();
	}
	ShaderDataType* STStruct::GetElement(uint32_t sorting_order)
	{
		if (m_SortingOrderMap.find(sorting_order) == m_SortingOrderMap.end())
			return nullptr;
		return m_SortingOrderMap[sorting_order].first;
	}
	std::string STStruct::GetElementName(uint32_t sorting_order)
	{
		if (m_SortingOrderMap.find(sorting_order) == m_SortingOrderMap.end())
			return "None";
		return m_SortingOrderMap[sorting_order].second;
	}
	bool STStruct::IsElementStruct(const std::string& var_name) 
	{
		if (m_StructElements.find(var_name) == m_StructElements.end())
			return false;

		return m_StructElements[var_name]->m_Type == JGShader::_struct;
		
	}
	STStruct* STStruct::GetElementStruct(const std::string& var_name) 
	{
		if (m_StructElements.find(var_name) == m_StructElements.end())
			return nullptr;
		if (!IsElementStruct(var_name))
			return nullptr;
		return (STStruct * )m_StructElements[var_name].get();
	}
	std::string STStruct::GetShaderDefineCode() const
	{
		std::string code = "struct " + GetName() + "\n{\n";

		for (auto& sdtype : m_SortingOrderMap)
		{
			auto type = sdtype.second.first;
			auto name = sdtype.second.second;
			code += "   " + type->GetName() + " " + name + ";\n";
		}

		code += "};\n";

		return code;
	}

	void STStruct::SetElementName(const std::string& var_name, const std::string& set_name)
	{
		if (m_StructElements.find(var_name) == m_StructElements.end())
			return;

		auto data = m_StructElements[var_name];
		m_StructElements.erase(var_name);
		m_StructElements[set_name] = data;
		m_SortingOrderMap[data->m_SortingOrder].second = set_name;
	}
	void STStruct::SetElementSortingOrder(const std::string& var_name, uint32_t sorting_order)
	{
		if (m_StructElements.find(var_name)       == m_StructElements.end() ||
			m_SortingOrderMap.find(sorting_order) == m_SortingOrderMap.end() )
			return;

		auto type = m_StructElements[var_name];

		auto value  = m_SortingOrderMap[sorting_order]; // -> value2
		auto value2 = m_SortingOrderMap[type->m_SortingOrder]; // -> value
		

		m_SortingOrderMap[sorting_order] = value2;
		m_SortingOrderMap[type->m_SortingOrder] = value;


		value.first->m_SortingOrder = type->m_SortingOrder;
		value2.first->m_SortingOrder = sorting_order;
	}
	std::vector<jbyte> STStruct::GetByteData() const
	{
		std::vector<jbyte> result;
		for (auto& sdtype : m_SortingOrderMap)
		{
			auto type = sdtype.second.first;
			auto btdata = type->GetByteData();
			result.insert(result.end(), btdata.begin(), btdata.end());
		}
		return result;
	}
	void STStruct::Save(std::ofstream& fout)
	{
		ShaderDataType::Save(fout);
		DataIO::write(fout, m_SortingIndex);
		DataIO::write(fout, GetElementCount());
		for (auto& data : m_SortingOrderMap)
		{
			DataIO::write(fout, data.second.first->GetType());
			DataIO::write(fout, data.second.second);
			data.second.first->Save(fout);
		}
	}
	void STStruct::Load(std::ifstream& fin)
	{
		ShaderDataType::Load(fin);
		string name;
		uint32_t count;
		DataIO::read(fin, name);
		DataIO::read(fin, m_SortingIndex);
		DataIO::read(fin, count);
		for (uint32_t i = 0; i < count; ++i)
		{
			JGShader::EShaderData type;
			std::string element_name;
			DataIO::read(fin, type);
			auto element = GetShaderDataTypeByType(type);
			DataIO::read(fin, element_name);
			element->Load(fin);
		}
	}
}