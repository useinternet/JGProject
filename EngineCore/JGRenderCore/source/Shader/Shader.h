#pragma once
#include"DxCommon/DxCommon.h"
#include"DxCore/EnginePipeLineStateDescList.h"
#include"ShaderRootSignature.h"
#include"Shader/ShaderPath.h"
// 매크로 정의
#define SHADER_MACRO_ONLY_DEFINE 1
#define SHADER_MACRO_DEFINE_REFLECTION  "REFLECTION"
#define SHADER_MACRO_DEFINE_REFRACTION  "REFRACTION"
#define SHADER_MACRO_DEFINE_SHADER_INSTANCE_OBJECT "SHADER_INSTANCE_OBJECT"

#define SHADER_MACRO_DEFINE_USING_DIFFUSEMAP "USING_DIFFUSEMAP"
#define SHADER_MACRO_DEFINE_USING_NORMALMAP  "USING_NORMALMAP"

#define SHADER_MACRO_DEFINE_SKINNED          "SKINNED"
#define SHADER_MACRO_DEFINE_SKINNED_MAX_BONE "MAX_BONE"
namespace JGRC
{
	enum class RCORE_EXPORT EPSOMode
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
	enum class RCORE_EXPORT EShaderType
	{
		Vertex,
		Pixel,
		Geometry,
		Hull,
		Domain,
		Compute
	};
	typedef struct RCORE_EXPORT ShaderMacroPack
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
		std::vector<ShaderMacroPack> m_Macro;
	protected:
		const std::string VS = "VS";
		const std::string PS = "PS";
		const std::string GS = "GS";
		const std::string HS = "HS";
		const std::string DS = "DS";
		const std::string Version = "5_1";
	public:
		Shader() = default;
		Shader(const std::wstring& shaderPath, const ShaderTypeInformation& ShaderTypes);
		void SetPath(const std::wstring& shaderPath, const ShaderTypeInformation& ShaderTypes);
		void Macro_Push(const std::string& define, UINT value);
		void Macro_Merge(const std::vector<ShaderMacroPack>& v);
		Microsoft::WRL::ComPtr<ID3D12PipelineState> CompileAndConstrutPSO(EPSOMode mode, ShaderRootSignatureBase* RootSig);
	private:
		void Compile(ShaderRootSignatureBase* RootSig);
		void CreateModeDesc(EPSOMode mode, D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc);
	};

	class RCORE_EXPORT CommonPSOPack
	{
	private:
		Shader CustomCompiler;
		Shader ShadowCompiler;
		Shader ViewNormalCompiler;
	public:
		CommonPSOPack() = default;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> CustomPSO;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> ShadowPSO;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> ViewNormalPSO;
		void CompilePSO(const std::wstring& path, EPSOMode CustomMode, ShaderRootSignatureBase* RootSig)
		{
			CustomCompiler.SetPath(path, { EShaderType::Vertex, EShaderType::Pixel });
			ShadowCompiler.SetPath(global_shadow_hlsl_path, { EShaderType::Vertex, EShaderType::Pixel });
			ViewNormalCompiler.SetPath(global_drawnormal_hlsl_path, { EShaderType::Vertex, EShaderType::Pixel });
			
			CustomPSO     = CustomCompiler.CompileAndConstrutPSO(CustomMode, RootSig);
			ShadowPSO     = ShadowCompiler.CompileAndConstrutPSO(EPSOMode::SHADOW, RootSig);
			ViewNormalPSO = ViewNormalCompiler.CompileAndConstrutPSO(EPSOMode::SSAO_NORMALMAP, RootSig);
		}
		void Macro_Push(const std::string& define, UINT value)
		{
			CustomCompiler.Macro_Push(define, value);
			ShadowCompiler.Macro_Push(define, value);
			ViewNormalCompiler.Macro_Push(define, value);
		}
		void Macro_Merge(const std::vector<ShaderMacroPack>& v)
		{
			CustomCompiler.Macro_Merge(v);
			ShadowCompiler.Macro_Merge(v);
			ViewNormalCompiler.Macro_Merge(v);
		}
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

