#include<PCH.h>
#include"ReflectionMap.h"
#include"DirectXToolKit/DirectXCore.h"
#include"DirectXToolKit/CommandList.h"
#include"GBuffer.h"
#include"Scene.h"
using namespace Dx12;
using namespace std;


ReflectionMap::ReflectionMap(
	uint32_t width, uint32_t height,
	float farZ, float NearZ, bool enableMipmap, DXGI_FORMAT format) :
	m_EnableMipMaps(enableMipmap),
	CubeMap(width, height, farZ, NearZ, format)
{
	Build();
}
void ReflectionMap::Draw(CommandList* commandList, const std::vector<Dx12Object*>& objArray)
{
	for (int i = 0; i < 6; ++i)
	{
		m_GBuffer->Draw(commandList, objArray, m_PassCB[i]);
		m_Scene[i]->Draw(commandList, m_GBuffer.get());
	}
}
void ReflectionMap::Resize(uint32_t width, uint32_t height)
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
const Texture& ReflectionMap::GetTexture() const
{
	return m_Scene[0]->GetRenderTarget().GetTexture(RtvSlot::Slot_0);
}
void ReflectionMap::Build()
{
	CubeMap::Build();
	ResourceBuild();
}
void ReflectionMap::ResourceBuild()
{
	RenderTarget rendertarget;
	(m_EnableMipMaps) ? 
		rendertarget = CreateRenderTarget("ReflectionMap") :
		rendertarget = CreateRenderTarget("ReflectionMap", 1);
	

	for (int i = 0; i < 6; ++i)
	{
		m_Scene[i] = make_unique<Scene>(m_Width, m_Height, rendertarget, m_Format);
		m_Scene[i]->GetRenderTarget().SetRenderTargetDesc(RtvSlot::Slot_0, &m_RTVDescs[i]);
		m_Scene[i]->GetRenderTarget().SetDepthStencilDesc(&m_DSVDescs[i]);
	}
	m_GBuffer = make_unique<GBuffer>(m_Width, m_Height);
}
