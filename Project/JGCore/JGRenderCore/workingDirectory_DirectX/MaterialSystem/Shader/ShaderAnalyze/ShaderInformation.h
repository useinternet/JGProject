#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
namespace JGRC
{		
	namespace hlslType
	{
		//
		static const std::string DEFINE  = "#define";
		static const std::string INCLUDE = "#include";
		static const std::string CBUFFER = "cbuffer";
		// 입력 레이아웃
		static const std::string INPUTLAYOUT = "INPUTLAYOUT";
		static const std::string INPUTSLOT   = "INPUTSLOT";
		// 타입
		static const std::string INSTNACE  = "INSTANCE";
		static const std::string TEXTURE2D = "Texture2D";
		static const std::string FLOAT     = "float ";
		static const std::string FLOAT2    = "float2 ";
		static const std::string FLOAT3    = "float3 ";
		static const std::string FLOAT4    = "float4 ";
		static const std::string MATRIX    = "matrix ";


		// 샘플러
		static const std::string SAMPLER_Start         = "CONFIG_START";
		static const std::string SAMPLER_Filter        = "Filter";
		static const std::string SAMPLER_TextureMode   = "TextureMode";
		static const std::string SAMPLER_MipLODBias    = "MipLODBias";
		static const std::string SAMPLER_MaxAnisotropy = "MaxAnisotropy";
		static const std::string SAMPLER_BoderColor    = "BoderColor";
		static const std::string SAMPLER_MinLOD        = "MinLOD";
		static const std::string SAMPLER_MaxLOD        = "MaxLOD";
		static const std::string SAMPLER_End           = "CONFIG_END";
		// 샘플러 모드
		static const std::string SAMPLER_DefaultMode_Wrap  = "DEFAULT_WRAPMODE";
		static const std::string SAMPLER_DefaultMode_Clamp = "DEFAULT_CLAMPMODE";

		// 텍스쳐
		static const std::string TEXTURE_Start  = "TEXTURE_START";
		static const std::string TEXTURE_End    = "TEXTURE_END";
		static const std::string TEXTURE_Config = "TEXTURE_CONFIG";
	}
	//
	class CORE_EXPORT ShaderInformation
	{
	protected:

	private:
		bool m_Progressing;
	public:
		ShaderInformation() {}
		virtual ~ShaderInformation() {}

		bool IsProgressing() const { return m_Progressing; }
		virtual void AnalyzeSentence(std::string& sentence) {};
		virtual bool Decryptable(const std::string& sentence) { return false; };

	protected:
		inline void Start() { m_Progressing = true; }
		inline void Stop() { m_Progressing = false; }
		uint ExtractionBracketNumber(const std::string& s);
		float ExtractionBracketFloat(const std::string& s);
		void ExtractionBracketComaFloat(const std::string& s, float* outPut);
		void ExtractionBracketComaNumber(const std::string& s, uint* outPut);
	};

}


