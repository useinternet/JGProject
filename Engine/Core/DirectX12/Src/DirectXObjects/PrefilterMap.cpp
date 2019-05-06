#include<PCH.h>
#include"PrefilterMap.h"
#include"Dx12Object.h"
#include"DirectXToolKit/GeometryGenerator.h"
#include"DirectXToolKit/DirectXCore.h"
using namespace Dx12;
using namespace std;



PrefilterMap::PrefilterMap(
	CommandList* commandList,
	uint32_t width, uint32_t height,
	float farZ, float NearZ) :
	CubeMap(width, height, farZ, NearZ, DXGI_FORMAT_R16G16B16A16_FLOAT)
{
	Build();
	m_Object = make_unique<Dx12Object>();
	m_Object->SetMesh(GeometryGenerator::CreateBox(commandList, 10000.0f, 10000.0f, 10000.0f, 0));
	m_Object->AddCubeTexture(Texture());
	m_Object->Build(PREFILTERMAP_SHADER_PATH, { ShaderStage::Vertex, ShaderStage::Pixel },
		PreparedPSO::ProfilterMap);
	m_Object->Update();
}
void PrefilterMap::Draw(CommandList* commandList, const Texture& CubeTexture)
{
	m_Object->SetCubeTexture(CubeTextureSlot::Slot0, CubeTexture);
	m_Object->Update();
	float mipWidth = m_Viewport.Get().Width;
	float mipHeight = m_Viewport.Get().Height;

	commandList->SetGraphicsRootSignature(DxDevice::GetShaderCommonDefines()->GetMainRootSig());
	for (int mip = 0; mip < ms_MaxMipLevels; ++mip)
	{

		mipWidth = (float)m_Width * pow(0.5, mip);
		mipHeight = (float)m_Height * pow(0.5, mip);

		m_Viewport.Set(mipWidth, mipHeight);
		m_ScissorRect.Set((int)mipWidth, (int)mipHeight);

		commandList->SetViewport(m_Viewport);
		commandList->SetScissorRect(m_ScissorRect);

		float roughness = (float)mip / (float)(ms_MaxMipLevels - 1);

		for (int i = 0; i < ms_NumFace; ++i)
		{
			m_PassCB[i].padding.x = roughness;
			m_RTVDescs[i].Texture2DArray.MipSlice = mip;

			m_RenderTarget.SetRenderTargetDesc(RtvSlot::Slot_0, &m_RTVDescs[i]);
			m_RenderTarget.SetDepthStencilDesc(&m_DSVDescs[i]);
			commandList->ClearRenderTarget(m_RenderTarget);
			commandList->SetRenderTarget(m_RenderTarget);

			commandList->SetGraphicsConstantBufferView(CommonRootParam::PassCB, &m_PassCB[i]);
			commandList->SetGraphicsShaderResourceView(CommonRootParam::MaterialCB, Material::GetMaterialCBArray());
			m_Object->Draw(commandList);
		}
	}
	m_Viewport.Set((float)m_Width, (float)m_Height);
	m_ScissorRect.Set(m_Width, m_Height);
}
const Texture& PrefilterMap::GetTexture() const
{
	return m_RenderTarget.GetTexture(RtvSlot::Slot_0);
}
void PrefilterMap::Build()
{
	CubeMap::Build();
	m_RenderTarget = CreateRenderTarget("PrefilterMap", ms_MaxMipLevels);
	m_Viewport.Set((float)m_Width, (float)m_Height);
	m_ScissorRect.Set(m_Width, m_Height);
}