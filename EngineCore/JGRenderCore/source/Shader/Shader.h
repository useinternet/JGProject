#pragma once
#include"DxCommon/DxCommon.h"
#include"DxCore/EnginePipeLineStateDescList.h"
#include"ShaderRootSignature.h"
// 매크로 정의
#define SHADER_MACRO_DEFINE_TEXTURE_MAX "MAX_TEXTURE"
#define SHADER_MACRO_DEFINE_REFLECTION  "REFLECTION"
#define SHADER_MACRO_DEFINE_REFRACTION  "REFRACTION"
#define SHADER_MACRO_DEFINE_SHADER_INSTANCE_OBJECT "SHADER_INSTANCE_OBJECT"
namespace JGRC
{
	enum class EPSOMode
	{
		DEFAULT,
		SCENE,
		SKYBOX,
		SHADOW,
		SSAO,
		SSAO_NORMALMAP,
		ONETARGET_DEFAULT,
		INSTANCE,
		COUNT
	};
	enum class EMacroMode
	{
		AllOff = 0,
		Reflection,
		Refraction
	};
	enum class RCORE_EXPORT EShaderType
	{
		Vertex,
		Pixel,
		Geometry,
		Hull,
		Domain,
		Compute
	};
	typedef struct ShaderMacroPack
	{
		std::string Macro;
		std::string Value;
	}ShaderMacroPack;
	typedef std::vector<EShaderType> ShaderTypeInformation;
	typedef Microsoft::WRL::ComPtr<ID3DBlob>               ShaderBtCode;
	typedef std::unordered_map<EShaderType, ShaderBtCode>  ShaderBtCodeArray;
	class RCORE_EXPORT Shader
	{
		std::wstring m_ShaderPath;
		ShaderTypeInformation        m_ShaderTypeInfor;
		ShaderBtCodeArray            m_ShaderBtCodes;
		std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_PSOList;
	protected:
		const std::string VS = "VS";
		const std::string PS = "PS";
		const std::string GS = "GS";
		const std::string HS = "HS";
		const std::string DS = "DS";
		const std::string Version = "5_1";
	public:
		void Init(const std::wstring& shaderPath, const ShaderTypeInformation& ShaderTypes);
		const std::wstring& GetPath() { return m_ShaderPath; }
		ID3D12PipelineState* CompileAndConstrutPSO(EPSOMode mode, ShaderRootSignatureBase* RootSig, const std::vector<ShaderMacroPack>& Macros);
	private:
		void Compile(ShaderRootSignatureBase* RootSig, const std::vector<ShaderMacroPack>& Macros);
		void CreateModeDesc(EPSOMode mode, D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc);
	};

	class RCORE_EXPORT ComputeShader
	{
	private:
		ComputeShader(const ComputeShader& rhs) = delete;
		ComputeShader& operator=(const ComputeShader& rhs) = delete;
	private:
		const std::string Version = "5_1";
		class DxCore* m_DxCore = nullptr;
		std::wstring m_ShaderPath;
		std::unordered_map<std::string, ShaderBtCode> m_Shaders;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>>  m_PSOList;
	public:
		ComputeShader() = default;
		void Init(DxCore* Core, ShaderRootSignatureBase* RootSig, const std::wstring& shaderPath,
			const std::vector<std::string>& functionName);
		ID3D12PipelineState* GetPSO(const std::string& name);
	private:
		void ShaderCompile(const std::vector<std::string>& functionName);
		void CreatePSO(ShaderRootSignatureBase* RootSig);
	};

}

