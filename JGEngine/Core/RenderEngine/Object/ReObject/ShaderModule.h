#pragma once




#include "Object/ReObject.h"
#include "Object/DxObject/Shader.h"
#include "Object/ReObject/ShaderCode.h"
namespace RE
{
	class RootSignature;
	class ShaderRegisterMap;
	class CommandList;
	class RenderTarget;
	class GraphicsPipelineState;
	class PipelineState;
	class ShaderModule : public ReObject
	{

		using ShaderCodePool = std::unordered_map<std::string, std::shared_ptr<ShaderCode>>;
		using ShaderDataPool = std::unordered_map<std::string, std::shared_ptr<ShaderData>>;
		using DefinedVarArray = std::vector<SCVar*>;
		using DefinedStructArray = std::vector<SCStruct*>;
		using DefinedInputStructArray = std::vector<SCInputStruct*>;
		using BindedConstantBufferArray = std::map<std::string, SDConstantBuffer*>;
		using BindedResourceArray = std::map<std::string, SDResource*>;
		using BindedSamplerStateArray = std::map<std::string, SDSamplerState*>;
		using BindedStructuredBufferArray = std::map<std::string, SDStructuredBuffer*>;
		using ErrorCodeMap = std::map<ShaderType, std::vector<std::string>>;
		using InputOutputStream = std::pair<std::string, std::string>;
		using RootParamMap = std::map<std::string, uint32_t>;
	public:
		ShaderModule();


		// 모듈에서 셰이더코드를 생성
		SCVar*              DefineVar(ShaderEnum::EShaderDataType type, const std::string& name, ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);
		SCStruct*           DefineStruct(const std::string& name, ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);
		SCInputStruct*      DefineInputStruct(const std::string& name, ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);

		SDConstantBuffer*   DefineConstantBuffer(
			const std::string& name,
			ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);
		SDStructuredBuffer* DefineStructuredBuffer(
			const std::string& name, 
			const std::string& defined_struct_name,
			ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);
		SDResource*         DefineSDResource(
			ShaderEnum::EShaderResourceType type,
			const std::string& name,
			uint32_t init_array_size = 1,
			ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);

		SDSamplerState*     DefineSamplerState(
			const std::string& name,
			const D3D12_STATIC_SAMPLER_DESC& desc,
			ShaderEnum::EShaderVisible visible = ShaderEnum::Visible_All);


		// Find 관련
		SCVar*         FindDefinedVar(const std::string& name);
		SCStruct*      FindDefinedStruct(const std::string& name);
		SCInputStruct* FindDefinedInputStruct(const std::string& name);

		SDConstantBuffer*   FindBindedContantBuffer(const std::string& name);
		SDStructuredBuffer* FindBindedStructuredBuffer(const std::string& name);
		SDSamplerState*     FindBindedSamplerState(const std::string& name);
		SDResource*         FindBindedSDResource(const std::string& name);



		void UnDefine(const std::string& name);
		void UnBind(const std::string& name);


		void SetInputOutputStream(
			ShaderType type, 
			const std::string& in_inputstruct_name,
			const std::string& out_inputstruct_name);
	protected:
		virtual void PushBindedDataInCommandList(CommandList* cmdList) = 0;
	public:
		virtual void PushCode(ShaderType type, const std::string& code) = 0;
		virtual void CreateRootSignature();
		virtual void CreateCode() = 0;
		virtual void Execute(CommandList* cmdList, const PipelineState& pso) = 0;
		bool Compile(ErrorCodeMap& out_error_code = ErrorCodeMap());

		//
		virtual std::string GetCode(ShaderType type);
		RootSignature* GetRootSignature() const;
		ShaderRegisterMap* GetRegisterMap() const;
		const Shader& GetShader(ShaderType type) const;
	protected:
		bool DuplicateCheck(const std::string& name);
	protected:
		struct DataByShaderType
		{
			std::string             MainCode;
			std::string             FinalCode;
			std::shared_ptr<Shader> Data;
			ShaderCompiler          Compiler;
			InputOutputStream       Stream;
		};
		// 정의 코드 조각
		DefinedVarArray         m_DefinedShaderVars;
		DefinedStructArray      m_DefinedShaderStruct;
		DefinedInputStructArray m_DefinedShaderInputStruct;

		// 바인딩 되는 데이터 코드
		BindedConstantBufferArray   m_BindedConstantBuffers;
		BindedStructuredBufferArray m_BindedStructuredBuffers;
		BindedResourceArray         m_BindedResources;
		BindedSamplerStateArray     m_BindedSamplerStates;

		// 셰이더 컴파일 및 코드 관련
		std::map<ShaderType, DataByShaderType> m_ShaderDatas;

		// 코드및 데이터 모음
		ShaderCodePool m_ShaderDefineCodes;
		ShaderDataPool m_ShaderBindDatas;

		// 루트 인덱스 맵
		RootParamMap m_ConstantBufferRootParamMap;
		RootParamMap m_StructuredBufferRootParamMap;
		RootParamMap m_ResourceRootParamMap;

		// 셰이더 레지스터 및 루트 서명 관리
		std::shared_ptr<ShaderRegisterMap>  m_RegisterMap;
		std::shared_ptr<RootSignature>      m_RootSignature;
	};

	class GraphicsShaderModule : public ShaderModule
	{
	protected:
		virtual void PushBindedDataInCommandList(CommandList* cmdList) override;
	public:
		GraphicsShaderModule(uint32_t width, uint32_t height) : ShaderModule() 
		{
			m_Viewport.Set((float)width, (float)height);
			m_ScissorRect.Set(width, height);
			m_Width  = width;
			m_Height = height;
		}
		virtual void PushCode(ShaderType type, const std::string& code) override;
		virtual void CreateCode() override;
		virtual void Execute(CommandList* cmdList, const PipelineState& pso) override;
	public:
		Texture* AddRenderTargetTexture(
			const std::string& name,
			DXGI_FORMAT format, 
			const Color& clearColor,
			uint16_t array_size = 1, 
			uint16_t miplevels = 0,
			uint32_t sampleCount = 1, 
			uint32_t sampleQuality = 0,
			uint64_t alignment = 0);
		Texture* AddDepthStencilTexture(
			const std::string& name,
			DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT,
			float clear_depth = 1, 
			uint8_t clear_stencil = 0,
			uint16_t array_size = 1,
			uint16_t miplevels = 0,
			uint32_t sampleCount = 1,
			uint32_t sampleQuality = 0,
			uint64_t alignment = 0,
			D3D12_CLEAR_FLAGS clear_flag = D3D12_CLEAR_FLAG_STENCIL | D3D12_CLEAR_FLAG_DEPTH);

		Texture* FindRenderTargetTexture(const std::string& name);
		Texture* FindRenderTargetTexture(uint32_t slot);
		RenderTarget* GetRenderTarget() const;

		std::shared_ptr<GraphicsPipelineState> GetPipelineState(
			const D3D12_DEPTH_STENCIL_DESC& depth_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT),
			const D3D12_RASTERIZER_DESC& raster_desc   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT),
			const D3D12_BLEND_DESC& blend_desc         = CD3DX12_BLEND_DESC(D3D12_DEFAULT));
	private:
		void InsertCodeByVisible(ShaderEnum::EShaderVisible visible,const std::string& src);

	private:
		// 해상도 관련
		Viewport    m_Viewport;
		ScissorRect m_ScissorRect;
		uint32_t    m_Width;
		uint32_t    m_Height;

		// 렌더 타겟 관련
		uint32_t    m_NumRenderTarget = 0;
		std::shared_ptr<RenderTarget>   m_RenderTarget;
		std::map<std::string, Texture*> m_RTTexturePool;
	};
	class ComputeShaderModule : public ShaderModule
	{




	};

}

