#pragma once
#include"DxCommon/DxCommon.h"

namespace JGRC
{
	class JGMaterial;
	struct MatPersonalData;
	class Interface_SceneMaterialData;
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
		Interface_SceneMaterialData AddData(const std::string& name);
	};
	class RCORE_EXPORT Interface_SceneMaterialData
	{
		friend class Interface_Scene;
	private:
		MatPersonalData* m_Data = nullptr;
	public:
		Interface_SceneMaterialData() = default;
		Interface_SceneMaterialData(MatPersonalData* data);
		Interface_SceneMaterialData* operator->()
		{
			return this;
		}
		void SetDiffuseAlbedo(float r, float g, float b, float a);
		void SetFresnelR0(float x, float y, float z);
		void SetRoughness(float x);
		void SetMetalic(float x);
		const std::string& GetName() const;
	};
	typedef Interface_SceneMaterialData IF_MaterialData;
	typedef Interface_SceneMaterial IF_Material;
}