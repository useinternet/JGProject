#pragma once
#include "Object/ReObject.h"
#include "Object/DxObject/Shader.h"
#include "Object/Shader/ShaderDefined.h"
namespace RE
{
	class CommandList;
	class SBDConstantBuffer;
	class SBDStructuredBuffer;
	class STStruct;
	class SBDTexture2D;
	class SBDTextureCube;
	class ShaderLibManager;
	class ShaderBindingData;
	class RootSignature;
	class Texture;
	class RenderTarget;
	class PipelineState;
	class GraphicsPipelineState;
	class ComputePipelineState;
	class ReCamera;

	enum class EModuleFormat : uint32_t
	{
		Compute,
		Graphics,


		G_StaticGBuffer,
		G_SkeletalGBuffer,
		G_GUI,
		G_Light,
		G_Quad,
	};
	//  루트 서명 작업 하다 맘 
	class ShaderModule : public ReObject
	{
		
	public:
		ShaderModule(const std::string& name, EModuleFormat format);
	public:
		SBDConstantBuffer*   AddConstantBuffer(const std::string& name);
		SBDStructuredBuffer* AddStructuredBuffer(const std::string& name, const std::string& strcut_type_name);
		void                 AddSamplerState(const std::string& name, const D3D12_STATIC_SAMPLER_DESC& desc);
		SBDTexture2D*        AddTexture2D(const std::string& name);
		SBDTextureCube*      AddTextureCube(const std::string& name);
		SBDConstantBuffer*         FindConstantBuffer(const std::string& name);
		SBDStructuredBuffer*       FindStructuredBuffer(const std::string& name);
		D3D12_STATIC_SAMPLER_DESC* FindSamplerState(const std::string& name);
		SBDTexture2D*              FindTexture2D(const std::string& name);
		SBDTextureCube*            FindTextureCube(const std::string& name);

		void SetMainCode(ShaderType shader_type, const std::string& code);
		void RemoveSBD(const std::string& name);
		void RemoveSS(const std::string& name);
	public:
		virtual bool Compile() = 0;
		virtual bool CustomCompile(std::shared_ptr<Shader>& shader, const std::string& main_code) = 0;
		virtual std::string GetCode(ShaderType type, const std::string& main_code) = 0;
		virtual void Execute(CommandList* cmdList) = 0;
		const std::string GetSafeCode(ShaderType type);

	public:
		std::string GetPath()
		{
			return "Shader\\Module\\" + GetName() + ".jg";
		}
		virtual bool Save(const std::string& path);
		virtual bool Load(const std::string& path);

		EModuleFormat GetModuleFormat() const
		{
			return m_ModuleFormat;
		}
	protected:
		virtual void DataSave(std::ofstream& fout);
		virtual void DataLoad(std::ifstream& fin);
		uint32_t GetRootParamIndex(const std::string& sbd_name);
	protected:
		using ShaderBindingDataPool = std::unordered_map<std::string, std::shared_ptr<ShaderBindingData>>;
		using SamplerStatePool      = std::unordered_map<std::string, D3D12_STATIC_SAMPLER_DESC>;

		struct ModuleData
		{
	
			ShaderCompiler          Compiler;
			std::shared_ptr<Shader> _Shader;
			std::string             MainCode;
			std::string             SafeCode;
			std::vector<JGShader::EShaderParameter> Params;
		};
		ShaderBindingDataPool   m_SBDPool;
		SamplerStatePool        m_SSPool;
		std::map<ShaderType, ModuleData> m_ModuleDatasByShaderType;
		std::shared_ptr<RootSignature>   m_RootSignature;
		EModuleFormat m_ModuleFormat;

		std::map<std::string, uint32_t>  m_RootParamMap;
	};
	class GraphicsShaderModule : public ShaderModule
	{
	public:
		GraphicsShaderModule(const std::string& name = "GraphicsShaderModule", EModuleFormat format = EModuleFormat::Graphics);
	public:
		void AddRenderTargetTexture(const std::string& name, DXGI_FORMAT format,  uint32_t miplevels = 0);
		void AddRenderTargetCubeTexture(const std::string& name, DXGI_FORMAT format, uint32_t miplevels = 0);
		void AddDepthStencilTexture(const std::string& name, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT, uint32_t miplevels = 0);
		void AddDepthStencilCubeTexture(const std::string& name, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT, uint32_t miplevels = 0);


		void AddInputEelement(ShaderType shader_type, JGShader::EShaderData type, const std::string& name, const std::string& semantic);
		void AddOutputEelement(ShaderType shader_type, JGShader::EShaderData type, const std::string& name, const std::string& semantic);
		void AddShaderParameter(JGShader::EShaderParameter param);


		void SetRenderTargetTextureName(uint32_t slot, const std::string& name);
		void SetRenderTargetTextureFormat(uint32_t slot, DXGI_FORMAT format);
		void SetDepthStencilTextureName(const std::string& name);
		void SetDepthStencilTextureFormat(DXGI_FORMAT format);

		void SetInputElementSementic(ShaderType type, const std::string& name, const std::string& sementic);
		void SetInputElementName(ShaderType type, const std::string& origin_name, const std::string& name);
		void SetInputElementDataType(ShaderType shader_type, const std::string& name, JGShader::EShaderData type);
		void SetOutputElementSementic(ShaderType type, const std::string& name, const std::string& sementic);
		void SetOutputElementName(ShaderType type, const std::string& origin_name, const std::string& name);
		void SetOutputElementDataType(ShaderType shader_type, const std::string& name, JGShader::EShaderData type);



		const Texture& GetRenderTargetTexture(uint32_t slot) const;

		void MakePSO(GraphicsPipelineState* pso);

		virtual bool MakeRootSignature();
		virtual void BindCamera(ReCamera* cam);

	public:
		virtual bool Compile() override;
		virtual bool CustomCompile(std::shared_ptr<Shader>& shader, const std::string& main_code) override;
		virtual std::string GetCode(ShaderType type, const std::string& main_code) override;
		virtual void Execute(CommandList* cmdList) override;
	protected:
		virtual void DataSave(std::ofstream& fout) override;
		virtual void DataLoad(std::ifstream& fin) override;
	protected:
		struct InputOutput
		{
			JGShader::EShaderData type;
			std::string name;
			std::string semantic;
		};

		uint32_t m_Width = 1920, m_Height = 1080;
		std::shared_ptr<RenderTarget>     m_RenderTarget;

		std::map<ShaderType, std::vector<InputOutput>> m_Input;
		std::map<ShaderType, std::vector<InputOutput>> m_Output;

		
		uint32_t m_NumRenderTarget = 0;
		ReCamera* m_BindedCamera = nullptr;
	};

	
	/*
	-- 추가 할것
	1. MaterialCB 생성
	
	
	*/

	class EntryShaderModule : public GraphicsShaderModule
	{
		static std::string GameObjectStructNameToBind;
		static std::string CameraStructNameToBind;
		static std::string GameObjectStructuredBufferName;
		static std::string CameraConstantBufferName;
		static std::string MaterialTextureArrayName;
		static std::string MaterialConstantBufferName;
	public:
		static const std::string& GameObjectSBName();
		static const std::string& CameraCBName();
		static const std::string& MatTextureArrayName();
		static const std::string& MatCBName();
		static const std::string& BindedGameObjectStructName();
		static const std::string& BindedCameraStructName();
		static void SetGameObjectSBName(const std::string& name);
		static void SetCameraCBName(const std::string& name);
		static void SetMatTextureArrayName(const std::string& name);
		static void SetMatConstantBufferName(const std::string& name);
		
	public:
		EntryShaderModule(const std::string& name, EModuleFormat format) :
			GraphicsShaderModule(name, format) {}
	public:
		virtual bool Load(const std::string& path) override;
		virtual void Init();
		virtual void BindCamera(ReCamera* cam) override;
		virtual void Execute(CommandList* cmdList) override;
	};

	class StaticGBufferModule : public EntryShaderModule
	{
	public:
		StaticGBufferModule(const std::string& name = "StaticGBufferModule");
	public:
		virtual void Init() override;
		virtual void Execute(CommandList* cmdList) override;
	private:

	};

	class SkeletalGBufferModule : public EntryShaderModule
	{
	public:
		SkeletalGBufferModule(const std::string& name = "SkeletalGBufferModule");
	public:
		virtual void Init() override {}
		virtual void Execute(CommandList* cmdList) override {}
	};

	class GUIModule : public EntryShaderModule
	{

	public:
		GUIModule(const std::string& name = "GUIModule");
	public:
		virtual void Init() override;
		virtual void Execute(CommandList* cmdList);

	};

	class ComputeShaderModule : public ShaderModule
	{
	public:

	};



	inline std::shared_ptr<GraphicsShaderModule> GetGModuleByFormat(EModuleFormat format)
	{
		switch (format)
		{
		case EModuleFormat::Graphics:
			return std::make_shared<GraphicsShaderModule>();
		case EModuleFormat::G_StaticGBuffer:
			return std::make_shared<StaticGBufferModule>();
		case EModuleFormat::G_SkeletalGBuffer:
			return std::make_shared<GraphicsShaderModule>();
		case EModuleFormat::G_Light:
			return std::make_shared<GraphicsShaderModule>();
		case EModuleFormat::G_Quad:
			return std::make_shared<GraphicsShaderModule>();
		case EModuleFormat::G_GUI:
			return std::make_shared<GUIModule>();
		default:
			return nullptr;
		}
	}
	inline std::shared_ptr<ComputeShaderModule> GetCModuleByFormat(EModuleFormat format)
	{

		return nullptr;
	}

	

}