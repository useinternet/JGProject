#pragma once
#include "Object/ReObject.h"
#include "Object/DxObject/Shader.h"
#include "Object/Shader/ShaderDefined.h"
namespace RE
{
	class SBDConstantBuffer;
	class SBDStructuredBuffer;
	class STStruct;
	class SBDTexture2D;
	class SBDTextureCube;
	class ShaderLibManager;
	class ShaderBindingData;
	class RootSignature;
	class RenderTarget;
	class PipelineState;
	class GraphicsPipelineState;
	class ComputePipelineState;
	class ShaderModule : public ReObject
	{
		
	public:
		ShaderModule(const std::string& name);
	protected:
		SBDConstantBuffer*   AddConstantBuffer(const std::string& name, ShaderType type);
		SBDStructuredBuffer* AddStructuredBuffer(const std::string& name, const std::string& strcut_type_name, ShaderType type);
		void                 AddSamplerState(const std::string& name, const D3D12_STATIC_SAMPLER_DESC& desc, ShaderType type);
		SBDTexture2D*        AddTexture2D(const std::string& name, ShaderType type);
		SBDTextureCube*      AddTextureCube(const std::string& name, ShaderType type);
		SBDConstantBuffer*         FindConstantBuffer(const std::string& name, ShaderType type);
		SBDStructuredBuffer*       FindStructuredBuffer(const std::string& name, ShaderType type);
		D3D12_STATIC_SAMPLER_DESC* FindSamplerState(const std::string& name, ShaderType type);
		SBDTexture2D*              FindTexture2D(const std::string& name, ShaderType type);
		SBDTextureCube*            FindTextureCube(const std::string& name, ShaderType type);

		void SetMainCode(ShaderType shader_type, const std::string& code);
		void RemoveSBD(const std::string& name, ShaderType type);
		void RemoveSS(const std::string& name, ShaderType type);
		virtual bool Compile() = 0;
	public:
		virtual std::string GetCode(ShaderType type) = 0;
	public:
		std::string GetPath()
		{
			return "Shader\\Module\\" + GetName() + ".jg";
		}
		virtual bool Save(const std::string& path);
		virtual bool Load(const std::string& path);
	protected:
		virtual void DataSave(std::ofstream& fout);
		virtual void DataLoad(std::ifstream& fin);
	protected:
		using ShaderBindingDataPool = std::unordered_map<std::string, std::shared_ptr<ShaderBindingData>>;
		using SamplerStatePool      = std::unordered_map<std::string, D3D12_STATIC_SAMPLER_DESC>;

		struct ModuleData
		{
			ShaderBindingDataPool   SBDPool;
			ShaderCompiler          Compiler;
			std::shared_ptr<Shader> _Shader;
			SamplerStatePool        SSPool;
			std::string             MainCode;
			std::string             SafeCode;
			std::vector<JGShader::EShaderParameter> Params;
		};

		std::map<ShaderType, ModuleData> m_ModuleDatasByShaderType;
		std::shared_ptr<RootSignature>   m_RootSignature;
	};

	enum EGModuleType
	{
		GBuffer,
		Light,
		Quad,
	};
	class GraphicsShaderModule : public ShaderModule
	{
	public:
		GraphicsShaderModule(const std::string& name);
	protected:
		void AddRenderTargetTexture(const std::string& name, DXGI_FORMAT format);
		void AddRenderTargetCubeTexture(const std::string& name, DXGI_FORMAT format);
		void AddDepthStencilTexture(const std::string& name, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);
		void AddDepthStencilCubeTexture(const std::string& name, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT);


		void AddInputEelement(ShaderType shader_type, JGShader::EShaderData type, const std::string& name, const std::string& semantic);
		void AddOutputEelement(ShaderType shader_type, JGShader::EShaderData type, const std::string& name, const std::string& semantic);
		void AddShaderParameter(JGShader::EShaderParameter param);


		void SetRenderTargetTextureName(const std::string& origin_name, const std::string& name);
		void SetRenderTargetTextureFormat(const std::string& name, DXGI_FORMAT format);
		void SetRenderTargetTextureName(uint32_t slot, const std::string& name);
		void SetRenderTargetTextureFormat(uint32_t slot, DXGI_FORMAT format);
		void SetDepthStencilTextureName(const std::string& name);
		void SetDepthStencilTextureFormat(DXGI_FORMAT format);

		void SetInputElementSementic(const std::string& name, const std::string& sementic);
		void SetInputElementName(const std::string& origin_name, const std::string& name);
		void SetInputElementDataType(const std::string& name, JGShader::EShaderData type);
		void SetOutputElementSementic(const std::string& name, const std::string& sementic);
		void SetOutputElementName(const std::string& origin_name, const std::string& name);
		void SetOutputElementDataType(const std::string& name, JGShader::EShaderData type);

		
		std::shared_ptr<GraphicsPipelineState> MakePipelineState() const;
		virtual bool Compile() override;
	public:
		virtual std::string GetCode(ShaderType type) override;
	protected:
		virtual void DataSave(std::ofstream& fout) override;
		virtual void DataLoad(std::ifstream& fin) override;
	private:
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

		std::map<std::string, uint32_t> m_TextureIndexMapByName;
		std::map<std::string, uint32_t> m_InputIndexMapByName;
		std::map<std::string, uint32_t> m_OuputIndexMapByName;

		
		uint32_t m_NumRenderTarget = 0;

	};

	class GBufferModule : public GraphicsShaderModule
	{
	public:
		GBufferModule(const std::string& name);
	public:
		virtual bool Load(const std::string& path);
	};




	class ComputeShaderModule : public ShaderModule
	{

	};




}