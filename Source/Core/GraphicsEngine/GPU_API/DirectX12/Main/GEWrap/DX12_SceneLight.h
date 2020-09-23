#pragma once
#include "SceneLight.h"
#include "SceneCamera.h"
#include "DX12_Texture.h"
#include "DirectXApi.h"



namespace DX12
{
	REGISTER_GRAPHICS_INTERFACE_DIRECTIONALLIGHT(DX12_DirectionalLight)
	class DX12_DirectionalLight : public GE::DirectionalLight
	{
		mutable std::unique_ptr<DX12_Texture> m_ShadowMaps;

		virtual GE::Texture* GetShadowMap() const override;
	public:
		virtual ~DX12_DirectionalLight() {}
	};

	REGISTER_GRAPHICS_INTERFACE_POINTLIGHT(DX12_PointLight)
	class DX12_PointLight : public GE::PointLight
	{
		mutable std::unique_ptr<DX12_Texture> m_ShadowCubeMap;
	public:
		virtual GE::Texture* GetShadowCubeMap() const override;
	public:
		virtual ~DX12_PointLight() {}
	};

	REGISTER_GRAPHICS_INTERFACE_SPOTLIGHT(DX12_SpotLight)
	class DX12_SpotLight : public GE::SpotLight
	{
		//
		// 뷰 텍스쳐
		// 그림자 텍스쳐
		mutable std::unique_ptr<DX12_Texture> m_ShadowMap;
	public:
		virtual GE::Texture* GetShadowMap() const override;
	public:
		virtual ~DX12_SpotLight() {}
	};


}