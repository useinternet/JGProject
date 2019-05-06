#include<PCH.h>
#include"IrradianceMap.h"
#include"Dx12Object.h"
#include"DirectXToolKit/DirectXCore.h"
#include"DirectXToolKit/GeometryGenerator.h"
#include"GBuffer.h"
using namespace Dx12;
using namespace std;
IrradianceMap::IrradianceMap(
	CommandList* commandList,
	uint32_t width, uint32_t height,
	float farZ, float NearZ) :
	CubeMap(width, height, farZ, NearZ, DXGI_FORMAT_R16G16B16A16_FLOAT) {
	Build();

	m_Object = make_unique<Dx12Object>();
	m_Object->SetMesh(GeometryGenerator::CreateBox(commandList, 10000.0f, 10000.0f, 10000.0f, 0));
	m_Object->AddCubeTexture(Texture());
	m_Object->Build(IRRADIANCEMAP_SHADER_PATH, { ShaderStage::Vertex, ShaderStage::Pixel },
		PreparedPSO::IrradianceMap);
	m_Object->Update();
}


void IrradianceMap::Draw(CommandList* commandList, const Texture& CubeTexture)
{
	m_Object->SetCubeTexture(CubeTextureSlot::Slot0, CubeTexture);
	m_Object->Update();
	commandList->SetViewport(m_Viewport);
	commandList->SetScissorRect(m_ScissorRect);

	commandList->SetGraphicsRootSignature(DxDevice::GetShaderCommonDefines()->GetMainRootSig());

	for (int i = 0; i < ms_NumFace; ++i)
	{
		m_RenderTarget.SetRenderTargetDesc(RtvSlot::Slot_0, &m_RTVDescs[i]);
		m_RenderTarget.SetDepthStencilDesc(&m_DSVDescs[i]);
		commandList->ClearRenderTarget(m_RenderTarget);
		commandList->SetRenderTarget(m_RenderTarget);
	
		commandList->SetGraphicsConstantBufferView(CommonRootParam::PassCB, &m_PassCB[i]);
		commandList->SetGraphicsShaderResourceView(CommonRootParam::MaterialCB, Material::GetMaterialCBArray());
		m_Object->Draw(commandList);
	}

}
const Texture& IrradianceMap::GetTexture() const 
{
	return m_RenderTarget.GetTexture(RtvSlot::Slot_0);
}

void IrradianceMap::Build()
{
	CubeMap::Build();
	m_RenderTarget = CreateRenderTarget("IrradianceMap");
	m_Viewport.Set((float)m_Width, (float)m_Height);
	m_ScissorRect.Set(m_Width, m_Height);
}