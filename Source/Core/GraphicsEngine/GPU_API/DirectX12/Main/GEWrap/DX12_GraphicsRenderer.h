#pragma once
#include "GraphicsRenderer.h"
#include "SceneObject.h"
#include "Material.h"
#include "DirectXApi.h"
#include "Pass/Graphics/GraphicPassDefine.h"
#include "Pass/Graphics/GraphicPasses.h"
namespace DX12
{
	REGISTER_GRAPHICS_INTERFACE_GRAPHICSRENDERER(DX12_GraphicsRenderer)

	class DX12_RenderingPass;
	class PassSetUpData;
	class DX12_GraphicsRenderer : public GE::GraphicsRenderer
	{

	public:
		DX12_GraphicsRenderer();
		virtual DX12_GraphicsRenderer::~DX12_GraphicsRenderer() override;
	public:
		virtual bool DrawCall(GE::Scene* scene) override;
		virtual bool CompileMaterial(GE::Material* mat) override;
		virtual void BeginFrame() override;
		virtual void EndFrame() override;
	public:
		bool GetPSO(GE::SceneObject* obj, GraphicsPipelineState& pso);
	private:
		struct SceneInfo
		{
			std::vector<GraphicPassMap*> passDatas;
			uint64_t dead_frame = 0;
		};
	private:
		void Init();

		bool MakePSO(GE::Material* mat, GE::ObjectType objectType);
		GraphicPassMap* CreatePassData();
	private:

		using PassDataPool = std::vector<std::unique_ptr<GraphicPassMap>>;
		using PassDataQueue = std::queue<GraphicPassMap*>;


		bool m_IsSuccessed = true;

		std::shared_mutex m_MaterialPSOsMutex;
		std::unordered_map<size_t, GraphicsPipelineState>    m_MaterialPSOs;
		GraphicsPipelineState m_ShadowStaticPSO;
		GraphicsPipelineState m_ShadowSkinnedPSO;


		// Pass Ãß°¡ 
		GraphicPassMap      m_OriginPassMap;
		PassDataPool        m_PassDataPool;
		PassDataQueue       m_WaitingPassData;

		std::unordered_map<GE::Scene*, SceneInfo> m_PassDataByScene;
		uint64_t m_DeadLine = 6;


		std::shared_mutex m_PassDataSceneMutex;
	};
}