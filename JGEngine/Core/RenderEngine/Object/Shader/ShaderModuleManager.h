#pragma once
#include "Object/ReObject.h"
#include "Object/DxObject/Shader.h"
#include "Object/Shader/ShaderDefined.h"

namespace RE
{
	class GraphicsShaderModule;
	class ComputeShaderModule;
	enum class EModuleFormat : uint32_t;
	class ShaderModuleManager : public ReObject
	{


	public:
		ShaderModuleManager(const std::string& name = "ShaderModuleManager") : ReObject(name)
		{ ReObject::m_ShaderModuleManager = this; }
		void Save(const std::string& path);
		void Load(const std::string& path);
	public:
		GraphicsShaderModule* CreateGraphicsModule(const std::string& name, EModuleFormat format);
		GraphicsShaderModule* FindGraphicsModule(const std::string& name);

		ComputeShaderModule*  CreateComputeModule(const std::string& name);
		ComputeShaderModule*  FindComputeModule(const std::string& name);
	private:
		std::unordered_map<std::string, std::shared_ptr<GraphicsShaderModule>> m_GraphicsModuleMap;
		std::unordered_map<std::string, std::shared_ptr<ComputeShaderModule>>  m_ComputeModuleMap;
	};
}
