#pragma once
#include"DxCommon/DxCommon.h"





namespace JGRC
{
	class RCORE_EXPORT CommonData
	{
	protected:
		static class Scene*     m_Scene;
		static class DxCore*    m_DxCore;
		static ResourceManager* m_ResourceManager;
	public:
		CommonData(DxCore* core, Scene* scene, ResourceManager* manager);
	public:
		static Scene* _Scene();
		static DxCore* _Core();
		static ID3D12Device* _Device();
		static ResourceManager* _ResourceManager();
	};
}