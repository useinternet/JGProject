#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	class JGMaterial;
	class RCORE_EXPORT Interface_SceneMaterial
	{
		friend class Interface_Scene;
	private:
		JGMaterial* m_Data = nullptr;
	public:
		Interface_SceneMaterial() = default;
		Interface_SceneMaterial(JGMaterial* mat);
		Interface_SceneMaterial* operator->()
		{
			return this;
		}
		void SetTexture(ETextureSlot slot, const std::wstring& TexturePath);
		void SetDiffuseAlbedo(float r, float g, float b, float a);
		void SetFresnelR0(float x, float y, float z);
		void SetRoughness(float x);
		void SetMetalic(float x);
	};

	typedef Interface_SceneMaterial IF_Material;
}