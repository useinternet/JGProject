#include "pch.h"
#include "ShaderModuleManager.h"
#include "ShaderModule.h"
using namespace std;


namespace RE
{
	void ShaderModuleManager::Save(const std::string& path)
	{
		for (auto& m : m_GraphicsModuleMap)
		{
			m.second->Save(path + "\\" + m.second->GetName() + JGFORMAT);
		}
		for (auto& m : m_ComputeModuleMap)
		{
			m.second->Save(path + "\\" + m.second->GetName() + JGFORMAT);
		}
	}
	void ShaderModuleManager::Load(const std::string& path)
	{
		auto entry = fs::directory_iterator(path);

		for (auto& file : entry)
		{
			if (file.path().extension() != JGFORMAT)
				continue;

		
			EAssetGroup  group;
			EAssetFormat format;
			EModuleFormat module_format;
			std::ifstream fin;
			fin.open(file.path().string());
			if (!fin.is_open())
				continue;


			DataIO::read(fin, group);
			DataIO::read(fin, format);
			DataIO::read(fin, module_format);


			auto m = GetGModuleByFormat(module_format);
			if (m)
			{
				m->Load(file.path().string());
				m_GraphicsModuleMap[m->GetName()] = m;
			}
			else
			{
				auto cm = GetCModuleByFormat(module_format);
				if (cm)
				{
					//auto m = make_shared<ComputeShaderModule>();
//m->Load(file.path().string());
//m_ComputeModuleMap[m->GetName()] = m;
				}
			}
			

			fin.close();
		}
	}

	GraphicsShaderModule* ShaderModuleManager::CreateGraphicsModule(const std::string& name, EModuleFormat format)
	{
		if (m_GraphicsModuleMap.find(name) != m_GraphicsModuleMap.end())
			return nullptr;
		shared_ptr<GraphicsShaderModule> m = GetGModuleByFormat(format);
		m->SetName(name);
		((StaticGBufferModule*)m.get())->Init();

		m_GraphicsModuleMap[name] = m;
		return m.get();
	}
	GraphicsShaderModule* ShaderModuleManager::FindGraphicsModule(const std::string& name)
	{
		if (m_GraphicsModuleMap.find(name) == m_GraphicsModuleMap.end())
			return nullptr;

		return m_GraphicsModuleMap[name].get();
	}

	ComputeShaderModule* ShaderModuleManager::CreateComputeModule(const std::string& name)
	{
		return nullptr;
	}
	ComputeShaderModule* ShaderModuleManager::FindComputeModule(const std::string& name)
	{
		return nullptr;
	}
}