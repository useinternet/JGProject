#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"ShaderObject.h"

namespace JGRC
{
	class CORE_EXPORT ShaderObjectManager
	{
	private:
		static ShaderObjectManager* Instance;
	private:
		std::vector<std::shared_ptr<ShaderObject>> m_vShaderObjects;

	private:
		ShaderObjectManager();
		~ShaderObjectManager();
		ShaderObjectManager(const ShaderObjectManager& copy) = delete;
		ShaderObjectManager& operator=(const ShaderObjectManager* SOM) = delete;
	public:
		static ShaderObjectManager* GetInstance();
		static void Release();
	public:
		ShaderObject* CreateShaderObject(EShaderType type);
	};
}
