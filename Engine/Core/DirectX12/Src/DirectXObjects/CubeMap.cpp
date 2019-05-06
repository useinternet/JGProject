#include<PCH.h>
#include"CubeMap.h"
using namespace Dx12;

CubeMap::CubeMap(uint32_t width, uint32_t height,
	float farZ, float NearZ, DXGI_FORMAT format) :
	m_Width(width), m_Height(height),
	m_FarZ(farZ), m_NearZ(NearZ), m_Format(format) {}

void CubeMap::SetPosition(const Common::JVector3& v)
{
	for (int i = 0; i < 6; ++i)
	{
		m_Camera[i].SetPosition(v);
	}
	PassCBBuild();
}
void CubeMap::SetCameraFarNear(float farZ, float nearZ)
{
	m_FarZ = farZ;
	m_NearZ = nearZ;
	for (int i = 0; i < 6; ++i)
	{
		m_Camera[i].SetLens(0.5f * JG_PI, (float)m_Width, (float)m_Height, m_NearZ, m_FarZ);
	}
}
void CubeMap::Build()
{
	CameraBuild();
	PassCBBuild();
}
RenderTarget CubeMap::CreateRenderTarget(const std::string& resourceName, int miplevels)
{
	RenderTarget rendertarget;
	D3D12_CLEAR_VALUE clearColor;
	clearColor.Format = m_Format;
	clearColor.Color[0] = 0.0f;
	clearColor.Color[1] = 0.0f;
	clearColor.Color[2] = 0.0f;
	clearColor.Color[3] = 1.0f;

	Texture texture = Texture(
		TextureUsage::RenderTarget,
		CD3DX12_RESOURCE_DESC::Tex2D(
			m_Format, m_Width, m_Height, 6, miplevels, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		&clearColor,
		resourceName);

	clearColor.DepthStencil.Depth = 1.0f;
	clearColor.DepthStencil.Stencil = 0;
	clearColor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	Texture depthTexture = Texture(
		TextureUsage::DepthStencil,
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			m_Width,
			m_Height,
			6, miplevels, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		&clearColor,
		resourceName + "Depth");


	for (int i = 0; i < ms_NumFace; ++i)
	{
		m_RTVDescs[i].Format = m_Format;
		m_RTVDescs[i].ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		m_RTVDescs[i].Texture2DArray.ArraySize = 1;
		m_RTVDescs[i].Texture2DArray.MipSlice = 0;
		m_RTVDescs[i].Texture2DArray.PlaneSlice = 0;
		m_RTVDescs[i].Texture2DArray.FirstArraySlice = i;


		m_DSVDescs[i].Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_DSVDescs[i].ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
		m_DSVDescs[i].Texture2DArray.ArraySize = 1;
		m_DSVDescs[i].Texture2DArray.FirstArraySlice = i;
		m_DSVDescs[i].Texture2DArray.MipSlice = 0;
	}

	rendertarget.AttachTexture(RtvSlot::Slot_0, texture);
	rendertarget.AttachTexture(RtvSlot::DepthStencil, depthTexture);

	rendertarget.SetDepthStencilClearColor(1.0f, 0, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);
	rendertarget.SetRenderTargetClearColor(RtvSlot::Slot_0, { 0.0f,0.0f,0.0f,1.0f });

	return rendertarget;
}
void CubeMap::CameraBuild()
{
	m_Camera[0].RotationYaw(90.0f); // +X
	m_Camera[1].RotationYaw(-90.0f); // -X;

	m_Camera[2].RotationPitch(-90.0f); // +Y;
	m_Camera[3].RotationPitch(90.0f); // -Y;

	m_Camera[4]; // +Z;
	m_Camera[5].RotationYaw(180.0f); // -Z
	float fov = 0.5f * JG_PI;
	for (int i = 0; i < ms_NumFace; ++i)
	{
		m_Camera[i].SetLens(fov, (float)m_Width, (float)m_Height, m_NearZ, m_FarZ);
	}
}
void CubeMap::PassCBBuild()
{
	for (int i = 0; i < ms_NumFace; ++i)
	{
		m_PassCB[i].Set(m_Camera[i]);
	}
}