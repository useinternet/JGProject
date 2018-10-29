#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderObject.h"
namespace JGRC
{
	// enum class EShaderType;
	class CORE_EXPORT ShaderAnalyzer
	{
	private:
		ShaderObject* Shader = nullptr;

	public:
		ShaderAnalyzer();
		~ShaderAnalyzer();

		bool Analyze(const std::string& hlslPath, const EShaderType ShaderType);
		


	private:
		std::string IncludeAyz(std::string hlslPath, std::string& sentence);
	};
}
