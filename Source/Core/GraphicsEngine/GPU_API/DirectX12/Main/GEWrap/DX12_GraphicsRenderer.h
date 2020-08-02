#pragma once
#include "GraphicsRenderer.h"
#include "SceneObject.h"
#include "Material.h"
#include "DirectXApi.h"

namespace DX12
{
	REGISTER_GRAPHICS_INTERFACE_GRAPHICSRENDERER(DX12_GraphicsRenderer)

	class DX12_RenderingPass;
	class PassSetUpData;
	class DX12_GraphicsRenderer : public GE::GraphicsRenderer
	{
	public:
		enum class RootParam
		{
			Camera, // ConstantBuffer
			Material, // ConstantBuffer
			AnimData,
			Object,
			InputTexture, 
			BindTexture,
			RootParam_Count
		};
	public:
		DX12_GraphicsRenderer();
		virtual DX12_GraphicsRenderer::~DX12_GraphicsRenderer() override;
	public:
		virtual bool DrawCall(GE::Scene* scene) override;
		virtual bool CompileMaterial(GE::Material* mat) override;
		virtual void BeginFrame() override;
		virtual void EndFrame() override;
	private:
		struct PassData
		{
			using PassPool = std::vector<std::unique_ptr<DX12_RenderingPass>>;
			using PassMap = std::unordered_map<std::string, DX12_RenderingPass*>;


			PassPool passPool;
			PassMap  passMap;
		};
		struct SceneInfo
		{
			std::vector<PassData*> passDatas;
			uint64_t dead_frame = 0;
		};
	private:
		void Init();
		bool GetPSO(GE::SceneObject* obj, GraphicsPipelineState& pso);
		bool MakePSO(GE::Material* mat, GE::ObjectType objectType);
		PassData* CreatePassData();
	private:
		using PassSetUpDataMap = std::unordered_map<std::string, PassSetUpData>;
		using PassDataPool     = std::vector<std::unique_ptr<PassData>>;
		using PassDataQueue    = std::queue<PassData*>;
		

		bool m_IsSuccessed = true;
		RootSignature m_RootSig;
		


		std::shared_mutex m_MaterialPSOsMutex;
		std::unordered_map<size_t, GraphicsPipelineState>    m_MaterialPSOs;


		// Pass Ãß°¡ 
		Concurrency::task_group m_PassSetUpTaskGroup;
		std::shared_mutex       m_SetUpMutex;
		PassSetUpDataMap        m_SetUpDataMap;

		PassDataPool        m_PassDataPool;
		PassDataQueue       m_WaitingPassData;

		std::unordered_map<GE::Scene*, SceneInfo> m_PassDataByScene;
		uint64_t m_DeadLine = 6;


		std::shared_mutex m_PassDataSceneMutex;
		std::shared_mutex m_PassDataQueueMutex;
		std::shared_mutex m_PassDataPoolMutex;
		
	};
}