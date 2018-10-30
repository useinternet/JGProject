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
		// hlsl path
		// 입력 레이아웃정보
		// 상수 버퍼
		std::unique_ptr<class LayoutInformation>  m_InputLayoutInfor;
		std::unique_ptr<class CBufferInformation> m_CBufferInfor;
		std::unique_ptr<class SamplerInformation> m_SamplerInfor;
		std::unique_ptr<class TextureInformation> m_TextureInfor;

		bool m_bIgnore = false;
	public:
		ShaderAnalyzer();
		~ShaderAnalyzer();

		bool Analyze(const std::string& hlslPath, const EShaderType ShaderType);
		


	private:
		bool RemoveRemark(std::string& sentence);
		std::string IncludeAyz(std::string hlslPath, std::string& sentence);
	};
}
