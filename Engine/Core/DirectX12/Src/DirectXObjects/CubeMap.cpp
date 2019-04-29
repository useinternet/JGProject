#include<PCH.h>
#include"CubeMap.h"
#include"DirectXToolKit/DirectXCore.h"
#include"DirectXToolKit/CommandList.h"
#include"GBuffer.h"
#include"Scene.h"
using namespace Dx12;
using namespace std;


CubeMap::CubeMap(
	uint32_t width, uint32_t height,
	float farZ, float NearZ, DXGI_FORMAT format) :
	m_Width(width), m_Height(height), m_FarZ(farZ), m_NearZ(NearZ), m_Format(format)
{
	Build();
}
void CubeMap::Draw(CommandList* commandList, const std::vector<Dx12Object*>& objArray)
{
	for (int i = 0; i < 6; ++i)
	{
		m_GBuffer->Draw(commandList, objArray, m_PassCB[i]);
		m_Scene[i]->Draw(commandList, m_GBuffer.get());
	}
}
void CubeMap::Resize(uint32_t width, uint32_t height)
{
	m_Width  = width;
	m_Height = height;
	m_GBuffer->Resize(width, height);
	for (int i = 0; i < 6; ++i)
	{
		m_Scene[i]->ReSize(m_Width, m_Height);
		m_Camera[i].SetLens(0.5f * JG_PI, (float)m_Width, (float)m_Height, m_NearZ, m_FarZ);
		m_PassCB[i].Set(m_Camera[i]);
	}

}
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
const Texture& CubeMap::GetTexture() const
{
	return m_Scene[0]->GetRenderTarget().GetTexture(RtvSlot::Slot_0);
}
void CubeMap::Build()
{
	ResourceBuild();
	CameraBuild();
	PassCBBuild();
}
void CubeMap::ResourceBuild()
{
	RenderTarget renserTarget;
	D3D12_CLEAR_VALUE clearColor;
	clearColor.Format = m_Format;
	clearColor.Color[0] = 0.0f;
	clearColor.Color[1] = 0.0f;
	clearColor.Color[2] = 0.0f;
	clearColor.Color[3] = 1.0f;


	Texture texture = Texture(
		TextureUsage::RenderTarget,
		CD3DX12_RESOURCE_DESC::Tex2D(
			m_Format, m_Width, m_Height, 6, 1, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		&clearColor,
		"CubeMapTexture");

	clearColor.DepthStencil.Depth = 1.0f;
	clearColor.DepthStencil.Stencil = 0;
	clearColor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	Texture depthTexture = Texture(
		TextureUsage::DepthStencil,
		CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			m_Width,
			m_Height,
			6, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		&clearColor,
		"CubeMapDepthTexture");


	for (int i = 0; i < 6; ++i)
	{
		m_RTVDescs[i].Format = m_Format;
		m_RTVDescs[i].ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
		m_RTVDescs[i].Texture2DArray.ArraySize = 1;
		m_RTVDescs[i].Texture2DArray.MipSlice = 0;
		m_RTVDescs[i].Texture2DArray.PlaneSlice = 0;
		m_RTVDescs[i].Texture2DArray.FirstArraySlice = i;
	}

	renserTarget.AttachTexture(RtvSlot::Slot_0, texture);
	renserTarget.AttachTexture(RtvSlot::DepthStencil, depthTexture);

	renserTarget.SetDepthStencilClearColor(1.0f, 0, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL);
	renserTarget.SetRenderTargetClearColor(RtvSlot::Slot_0, { 0.0f,0.0f,0.0f,1.0f });
	for (int i = 0; i < 6; ++i)
	{
		m_Scene[i] = make_unique<Scene>(m_Width, m_Height, renserTarget);
		m_Scene[i]->GetRenderTarget().SetRenderTargetDesc(RtvSlot::Slot_0, &m_RTVDescs[i]);
	}
	m_GBuffer = make_unique<GBuffer>(m_Width, m_Height);
}
void CubeMap::CameraBuild()
{
	m_Camera[0].RotationYaw(90.0f); // +X
	m_Camera[1].RotationYaw(-90.0f); // -X;

	m_Camera[2].RotationPitch(-90.0f); // +Y;
	m_Camera[3].RotationPitch(90.0f); // -Y;

	m_Camera[4]; // +Z;
	m_Camera[5].RotationYaw(180.0f); // -Z

	for (int i = 0; i < 6; ++i)
	{
		m_Camera[i].SetLens(0.5f * JG_PI, (float)m_Width, (float)m_Height, m_NearZ, m_FarZ);
	}
}
void CubeMap::PassCBBuild()
{
	for (int i = 0; i < 6; ++i)
	{
		m_PassCB[i].Set(m_Camera[i]);
	}
}