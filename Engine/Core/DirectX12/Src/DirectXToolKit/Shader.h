#pragma once
#include"DirectXCommon/DirectXCommon.h"
#define SHADER_MACRO_USE_SKINNED "USE_SKINNED"
#define SHADER_MACRO_DEFINE "1"
namespace Dx12
{
	class Shader
	{
	private:



	public:
		typedef ComPtr<ID3DBlob>              ShaderByteCode;
		typedef std::string                   ShaderCharCode;
		typedef std::vector<ShaderCharCode>   ShaderCharCodes;
		typedef std::vector<EShaderStage>     ShaderStages;
		typedef std::vector<D3D_SHADER_MACRO> D3DShaderMacro;
		typedef std::unordered_map<std::string, std::unordered_map<uint32_t, ShaderByteCode>> ShaderMap;
	protected:
		static ShaderMap ms_GlobalShaders;
		static std::map<EShaderStage, std::string> ms_EntryPoints;
		static std::map<EShaderStage, std::string> ms_HlslVersion;
		static std::mutex ms_GlobalShadersMutex;
		static std::mutex ms_EntryPointsMutex;
		static std::mutex ms_HlslVersionMutex;
		static bool ms_IsInitShaderConfig;
	protected:
		struct ShaderData
		{
			std::string    Path;
			ShaderCharCode CharCode;
			bool IsNonePath;

			std::string    Error;
			ShaderByteCode BtCode;
			D3DShaderMacro Macro;
		};
		std::vector<std::string> m_MacroStrMems;
		int m_MacroMemOffset  = 0;
		int m_MaxMacroMemSize = 0;
	public:
		Shader(int maxMacroSize);
		virtual ~Shader() = default;
	public:
		static void SetShaderDefaultConfig();
		static void SetEntryPoint(EShaderStage stage, const std::string& entryPoint);
		static void SetHLSLVersion(EHLSLVersion version);
	public:
		virtual bool Compile() = 0;
	};


	class GraphicsShader : public Shader
	{
	private:
		std::map<EShaderStage, ShaderData>    m_ShaderCodeDatas;
		bool m_IsCompile = false;
	public:
		GraphicsShader(int maxMacroSize = 1000);
		virtual ~GraphicsShader() = default;
	public:
		void AddMacro(
			EShaderStage stage, 
			const std::string& define,
			const std::string& value = "1");


		void AddMacros(
			const ShaderStages& stages,
			const std::string& define, 
			const std::string& value = "1");

		void AddShaderPath(EShaderStage stage, const std::string& path);
		void AddShaderPaths(const ShaderStages& stages, const std::string& path);

		void AddShaderCode(EShaderStage stage, const ShaderCharCode& code);
		void AddShaderCodes(const ShaderStages& stages, const ShaderCharCodes& codes);

		const void* GetShaderCode(EShaderStage stage) const;
		uint32_t    GetShaderCodeSize(EShaderStage stage) const;

		const std::string& GetErrorCode(EShaderStage stage) const;


		virtual bool Compile();
	};
	class ComputeShader : public Shader
	{
	private:
		std::map<std::string,ShaderData>  m_ShaderCodeDatas;
		bool m_IsCompile = false;
	public:
		ComputeShader(int maxMacroSize = 1000);
		virtual ~ComputeShader() = default;
	public:
		void AddMacro(
			const std::string& funcName,
			const std::string& define,
			const std::string value = "1");

		void AddMacros(
			const std::vector<std::string>& funcName,
			const std::string& define,
			const std::string value = "1");



		void SetShaderPathAndFunction(
			const std::string& path,
			const std::string& funcName);

		void SetShaderPathAndFunctions(
			const std::string& path,
			const std::vector<std::string>& funcNames);

		void SetShaderCode(const std::string& funcName, const ShaderCharCode& code);
		void SetShaderCode(const std::vector<std::string>& funcName, const ShaderCharCodes& code);

		const void* GetShaderCode(const std::string& funcName) const;
		uint32_t GetShaderCodeSize(const std::string& funcName) const;

		const std::string& GetErrorCode(const std::string& funcName) const;

		virtual bool Compile();
	};
}