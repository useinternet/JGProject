#pragma once
#include"DxCommon/DxCommon.h"
#include"SceneConfig.h"
#include"UserInterface/Interface_Scene.h"
class Camera;
namespace JGRC
{
	class DxDevice;
	class Scene;
	class ScreenManager;
	class GpuCpuSynchronizer;
	class CommandListManager;
	class RootSignatureManager;
	class DataManager;
	class RCORE_EXPORT JGRenderCore
	{
	private:
		std::shared_ptr<DxDevice>                   m_DxDevice;
		std::shared_ptr<ScreenManager>              m_ScreenManager;
		std::shared_ptr<GpuCpuSynchronizer>         m_GCS;
		std::shared_ptr<CommandListManager>         m_CmdListManager;
		std::shared_ptr<DataManager>                m_DataManager;
		std::shared_ptr<ResourceManager>            m_ResourceManager;
		std::shared_ptr<RootSignatureManager>       m_RootSigManager;
		std::shared_ptr<EngineFrameResourceManager> m_FrameResourceManager;
		std::shared_ptr<Scene> m_Scene;
	private:
		JGRenderCore(const JGRenderCore& copy) = delete;
		JGRenderCore& operator=(const JGRenderCore& copy) = delete;
	public:
		JGRenderCore();
		bool CreateCore();
		IF_Scene CreateScene(const std::string& SceneName, const SceneConfig& set);
		IF_Scene GetScene();
		void Build(const GameTimer& gt);
		void Update(const GameTimer& Timer);
		void ReSize(int width, int height);
		void Draw();
	};
}
