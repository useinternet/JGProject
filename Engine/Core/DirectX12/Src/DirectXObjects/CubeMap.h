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
	class Dx12Obejct;
	class CubeMap
	{

	protected:
		static const int ms_NumFace = 6;

		D3D12_RENDER_TARGET_VIEW_DESC m_RTVDescs[ms_NumFace];
		D3D12_DEPTH_STENCIL_VIEW_DESC m_DSVDescs[ms_NumFace];

		Camera m_Camera[6];
		PassCB m_PassCB[6];

	protected:
		uint32_t m_Width;
		uint32_t m_Height;
		float m_FarZ;
		float m_NearZ;
		DXGI_FORMAT m_Format;
	public:
		CubeMap(uint32_t width, uint32_t height,
			float farZ, float NearZ, DXGI_FORMAT format);



		// 큐브맵 그릴 위치 설정
		// @param v : 위치 값
		void SetPosition(const Common::JVector3& v);

		// 카메라 Near, Far 설정
		// @param farZ,nearZ : far, near 값
		void SetCameraFarNear(float farZ, float nearZ);

		//
		virtual const Texture& GetTexture() const = 0;
	protected:
		virtual void Build();
		RenderTarget CreateRenderTarget(const std::string& resourceName = "CubeMap", int miplevels = 0);
	private:
		void CameraBuild();
		void PassCBBuild();
	};
}