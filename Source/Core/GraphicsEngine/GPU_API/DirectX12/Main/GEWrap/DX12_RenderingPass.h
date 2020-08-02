#pragma once


#include "RenderingPass.h"
#include "Scene.h"
#include "DirectXApi.h"



namespace DX12
{
	class PassRunData;
	class DX12_RenderingPass;
	class DX12_GraphicsRenderer;
	using PassSetUpFunc = std::function<bool(RenderTarget&, GraphicsPipelineState&)>;
	using PassRunFunc   = std::function<bool(DX12_RenderingPass*, PassRunData&)>;

	class PassSetUpData
	{
	public:
		std::string   name;
		std::string   shaderPath;
		PassSetUpFunc setUpFunc;
		PassRunFunc   runFunc;
		GE::PassFlag  flag;
		DX12_GraphicsRenderer* ownerRenderer = nullptr;
	};

	class PassRunData
	{
	public:
		GraphicsCommandKeyPtr cmdKey;
		GE::Scene* scene = nullptr;
		std::vector<GE::SceneObject*> transparentObjects;
		void* passData = nullptr;
	};

	class DX12_RenderingPass : public GE::RenderingPass
	{
	public:
		// 패쓰 준비
		virtual bool SetUp(void* userData) override;
		// 
		// 패쓰 실행
		virtual bool Run(void* userData) override;




		virtual void  SetName(const std::string& name) override;
		virtual const std::string& GetName() const override;




		virtual ~DX12_RenderingPass() {}

	public:
		RenderTarget& GetRTRef() {
			return m_RenderTarget;
		}
		GraphicsPipelineState& GetPSORef() {
			return m_PSO;
		}
		const std::string& GetShaderPath() const {
			return m_ShaderPath;
		}
	private:
		std::string              m_Name;
		std::string              m_ShaderPath;
		GraphicsPipelineState    m_PSO;
		PassRunFunc              m_RunFunc;
		DX12_GraphicsRenderer*   m_OwnerRenderer = nullptr;
		RenderTarget m_RenderTarget;
	};
}