#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
#include"JGTexture.h"
namespace JGRC
{
	class CORE_EXPORT TextureManager
	{
	private:
		static TextureManager* Instance;
		std::vector<std::shared_ptr<JGTexture>> m_vTextures;
	private:
		TextureManager();
		~TextureManager();
	public:
		static TextureManager* GetInstance();
		static void Release();

		JGTexture* CreateTexture();
	};
}
