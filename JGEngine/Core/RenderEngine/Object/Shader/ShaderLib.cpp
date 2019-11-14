#include "pch.h"
#include "ShaderLib.h"
#include "ShaderDataType.h"

using namespace std;
namespace RE
{
	STStruct* ShaderTypeLib::AddStruct(const std::string& struct_name)
	{
		if (m_ShaderStructPool.find(struct_name) != m_ShaderStructPool.end())
		{
			return nullptr;
		}
		std::shared_ptr<STStruct> st = make_shared<STStruct>(struct_name);
		STStruct* result = st.get();
		st->m_SortingOrder = m_SortingIndex++;
		m_ShaderStructPool[struct_name] = st;
		m_SortingOrderMap[st->m_SortingOrder] = result;

		return result;
	}

	STStruct* ShaderTypeLib::FindStruct(const std::string& struct_name)
	{
		if (m_ShaderStructPool.find(struct_name) == m_ShaderStructPool.end())
		{
			return nullptr;
		}
		return m_ShaderStructPool[struct_name].get();
	}

	std::shared_ptr<STStruct>  ShaderTypeLib::CloneStruct(const std::string& struct_name)
	{
		auto st = FindStruct(struct_name);
		if (st)
		{
			return make_shared<STStruct>(*st);
		}
		return nullptr;
	}
	void ShaderTypeLib::SetSortingOrder(const std::string& struct_name, uint32_t sorting_order)
	{
		if (m_ShaderStructPool.find(struct_name) == m_ShaderStructPool.end())
		{
			return;
		}
		if (m_SortingOrderMap.find(sorting_order) == m_SortingOrderMap.end())
		{
			return;
		}

		auto v1 = m_ShaderStructPool[struct_name].get();
		auto v2 = m_SortingOrderMap[sorting_order];

		auto s1 = v1->m_SortingOrder;
		auto s2 = v2->m_SortingOrder;

		m_SortingOrderMap[s2] = v1;
		m_SortingOrderMap[s1] = v2;
	}
	void ShaderTypeLib::RemoveStruct(const std::string& struct_name)
	{
		if (m_ShaderStructPool.find(struct_name) == m_ShaderStructPool.end())
		{
			return;
		}

		uint32_t sorting_order = m_ShaderStructPool[struct_name]->m_SortingOrder;
		

		for (uint32_t i = sorting_order + 1; i < m_SortingIndex; ++i)
		{
			auto st = m_SortingOrderMap[i];

			m_SortingOrderMap.erase(i);
			m_SortingOrderMap[i - 1] = st;
			st->m_SortingOrder = i - 1;
		}
		--m_SortingIndex;

		m_ShaderStructPool.erase(struct_name);
	}
	bool ShaderTypeLib::Load(const std::string& path)
	{
		/*
1. Group, Format,  구조체 갯수
2. Struct 저장
   - Struct 이름 및 요소 갯수
   - Struct Var(Type, Type이름, Var이름)
3. 임시코드
4. 안정된 코드
*/
		std::ifstream fin(path, ios::binary);
		if (!fin.is_open())
		{
			return false;
		}
		EAssetGroup   group;
		EAssetFormat  format;
		size_t struct_count = 0;
		// 1번 
		DataIO::read(fin, group);
		DataIO::read(fin, format);
		DataIO::read(fin, struct_count);
		//fin >> group >> format >> struct_count;

		for (size_t i = 0; i < struct_count; ++i)
		{
			// 2번 (이름과 요소 갯수)
			std::string struct_name;
			uint32_t element_count = 0;


			DataIO::read(fin, struct_name);
			DataIO::read(fin, element_count);

			//fin >> struct_name >> element_count;

			std::shared_ptr<STStruct> st_struct = make_shared<STStruct>(struct_name);
			// 2번 구조체 변수( Type, type이름, var이름)
			for (uint32_t j = 0; j < element_count; ++j)
			{
				JGShader::EShaderData type;
				std::string type_name;
				std::string var_name;

				DataIO::read(fin, type);
				DataIO::read(fin, type_name);
				DataIO::read(fin, var_name);


				//fin >> type >> type_name >> var_name;


				if (type == JGShader::_struct)
				{
					if (m_ShaderStructPool.find(type_name) == m_ShaderStructPool.end())
					{
						STStruct st(type_name);
						st_struct->Add(st, var_name);
						m_IsSuccess = false;
					}
					else
					{
						st_struct->Add(*m_ShaderStructPool[type_name], var_name);
					}
		
				}
				else
				{
					st_struct->Add(type, var_name);
				}
			}
			m_ShaderStructPool[st_struct->GetName()] = st_struct;
			m_SortingOrderMap[i] = st_struct.get();
		}
		m_SortingIndex = (uint32_t)m_ShaderStructPool.size();
		if (m_IsSuccess)
		{
			m_SuccessCode = GetCode();
		}
		return true;
	}
	bool ShaderTypeLib::Save(const std::string& path)
	{
		/*
1. Group, Format, 구조체 갯수
   
2. Struct 저장
   - Struct 이름 및 요소 갯수
   - Struct Var(Type, Type이름, Var이름)
3. 임시코드
4. 안정된 코드
*/
		std::ofstream fout;
		fout.open(path, ios::binary);
		if (!fout.is_open())
		{
			return false;
		}
		// 1번
		// 그룹과 파일 포맷  구조체 갯수저장
		EAssetGroup  group  = EAssetGroup::AssetGroup_RE;
		EAssetFormat  format = EAssetFormat::AssetFormat_RE_ShaderTypeLib;
		DataIO::write(fout, group);
		DataIO::write(fout, format);
		DataIO::write(fout, m_ShaderStructPool.size());
		//fout << group << format << m_ShaderStructPool.size();


		for (auto& struct_pair : m_SortingOrderMap)
		{
			auto ststruct = struct_pair.second;


			// 2번 
			// 구조체 이름과 요소 갯수 저장
			DataIO::write(fout, ststruct->GetName());
			DataIO::write(fout, ststruct->GetElementCount());
			//fout << ststruct->GetName() << ststruct->GetElementCount();

			// 각 구조체 요소 저장
			for (uint32_t i = 0; i < ststruct->GetElementCount(); ++i)
			{
				auto element = ststruct->GetElement(i);


				DataIO::write(fout, element->GetType());
				DataIO::write(fout, element->GetName());
				DataIO::write(fout, ststruct->GetElementName(i));


				//fout << (uint32_t)element->GetType() << element->GetName() << ststruct->GetElementName(i);
			}

		}

		DataIO::write(fout, GetCode());
		DataIO::write(fout, m_SuccessCode);
		//fout << GetCode();
		//fout << m_SuccessCode;
		fout.close();
		return true;
	}
	std::string ShaderTypeLib::GetCode() const
	{

		std::string code =
			"#ifndef " + GetName() + "SHADERLIB \n" +
			"#define " + GetName() + "SHADERLIB \n";

		for (auto& st_struct_pair : m_SortingOrderMap)
		{

			auto ststruct = st_struct_pair.second;
			code += ststruct->GetShaderDefineCode();
		}

		code += "#endif";
		return code;
	}

	std::string ShaderTypeLib::GetSuccessedCode() const
	{
		if (m_SuccessCode.empty())
		{
			return GetCode();
		}
		return m_SuccessCode;
	}

	ShaderLibManager::ShaderLibManager() : ReObject("ShaderLibManager")
	{
		m_ShaderTypeLib = make_shared<ShaderTypeLib>();
		m_ShaderTypeLib->SetName("ShaderTypeLib");
		ReObject::m_ShaderLibManager = this;
	//	m_ShaderFuncLib = make_shared<ShaderFunctionLib>("ShaderFunctionLib");
	}


	void ShaderLibManager::Load(const std::string& path)
	{
		m_ShaderTypeLib->Load(path);
	}
	void ShaderLibManager::Save(const std::string& path)
	{
		m_ShaderTypeLib->Save(path);
	}

	ShaderTypeLib* ShaderLibManager::GetTypeLib()
	{
		return m_ShaderTypeLib.get();
	}
	std::string ShaderLibManager::GetCode() const
	{
		std::string code;
		code += m_ShaderTypeLib->GetCode();
		return code;
	}
}