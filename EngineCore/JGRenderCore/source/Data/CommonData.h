#pragma once
#include"DxCommon/DxCommon.h"





namespace JGRC
{
	class Scene;
	class ScreenManager;
	class DxCore;
	class RCORE_EXPORT CommonData
	{
	protected:
		static Scene*     m_Scene;
		static DxCore*    m_DxCore;
		static ScreenManager* m_ScreenManager;
		static ResourceManager* m_ResourceManager;
	public:
		CommonData(DxCore* core, Scene* scene, ResourceManager* manager, ScreenManager* screen);
	public:
		static Scene* _Scene();
		static DxCore* _Core();
		static ScreenManager* _ScreenManager();
		static ResourceManager* _ResourceManager();
	};
}