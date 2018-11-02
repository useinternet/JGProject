#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"MaterialSystem/Shader/ShaderTool/ShaderObject.h"

namespace JGRC
{
	// enum class EShaderType;
	class CORE_EXPORT ShaderAnalyzer
	{
		class AyzInformation
		{
		public:
			std::string hlslPath;
			std::shared_ptr<class LayoutInformation>  InputLayout;
			std::shared_ptr<class CBufferInformation> CBuffer;
			std::shared_ptr<class SamplerInformation> SamplerState;
			std::shared_ptr<class TextureInformation> Texture;

			AyzInformation();
		};
	private:
		std::map<EShaderType, AyzInformation> m_mInformation;
		bool m_bIgnore = false;
	public:
		bool Analyze(const std::string& hlslPath, const EShaderType ShaderType);

		// 나중에 이거 ofstream 매개변수로 받아서 이어서쓰기로.. ( 이거는 셰이더 관련만쓰는걸로..)
		bool OutputShaderData(const std::string& name);
	private:
		bool ReadShader(const std::string& hlslPath, AyzInformation* infor);
		bool RemoveRemark(std::string& sentence);
		std::string IncludeAyz(std::string hlslPath, std::string& sentence);
	};
}
