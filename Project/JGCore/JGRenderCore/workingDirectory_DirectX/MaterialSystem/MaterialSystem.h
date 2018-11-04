#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"Material.h"
namespace JGRC
{
	class CORE_EXPORT MaterialSystem
	{
		static MaterialSystem* Instance;
		std::vector<std::shared_ptr<Material>> m_vMaterials;
	private:
		MaterialSystem() {}
		~MaterialSystem() {}
	public:
		static MaterialSystem* GetInstance();
		static void Release();
		Material* CreateMaterial();
	};
}