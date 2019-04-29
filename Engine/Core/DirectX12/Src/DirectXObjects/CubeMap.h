#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"DirectXToolKit/RenderTarget.h"
#include"DirectXToolKit/Texture.h"
#include"DirectXToolKit/Camera.h"
#include"DirectXToolKit/Viewport.h"
#include"DirectXToolKit/ScissorRect.h"
#include"ShaderCommonDefines.h"

namespace Dx12
{
	class GBuffer;
	class Scene;
	class CommandList;
	class Dx12Object;
	class CubeMap
	{
	private:
		D3D12_RENDER_TARGET_VIEW_DESC m_RTVDescs[6];
		std::unique_ptr<GBuffer> m_GBuffer;
		std::unique_ptr<Scene>   m_Scene[6];
		Camera m_Camera[6];
		PassCB m_PassCB[6];
	private:
		uint32_t m_Width;
		uint32_t m_Height;
		float m_FarZ;
		float m_NearZ;
		DXGI_FORMAT m_Format;
	public:
		CubeMap(
			uint32_t width, uint32_t height,
			float farZ, float NearZ ,
			DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

		void Draw(CommandList* commandList, const std::vector<Dx12Object*>& objArray);
		void Resize(uint32_t width, uint32_t height);
		void SetPosition(const Common::JVector3& v);
		void SetCameraFarNear(float farZ, float nearZ);
		const Texture& GetTexture() const;
	private:
		void Build();
		void ResourceBuild();
		void CameraBuild();
		void PassCBBuild();
	};
}