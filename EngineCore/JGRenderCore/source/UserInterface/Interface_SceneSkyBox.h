#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	class JGRCObject;
	class RCORE_EXPORT Interface_SceneSkyBox
	{
		friend class Interface_Scene;
	private:
		JGRCObject* m_Data = nullptr;
	public:
		Interface_SceneSkyBox() = default;
		Interface_SceneSkyBox(JGRCObject* obj);
		Interface_SceneSkyBox* operator->()
		{
			return this;
		}
	};
	typedef Interface_SceneSkyBox IF_SkyBox;
}