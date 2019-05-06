#include<PCH.h>
#include"IntegratedBRDFTexture.h"
#include"ShaderCommonDefines.h"
#include"DirectXToolKit/Shader.h"
using namespace Dx12;
using namespace std;




const Texture& IntegratedBRDFTexture::GetTexture() const
{
	return m_RenderTarget.GetTexture(RtvSlot::Slot_0);
}



void IntegratedBRDFTexture::Build()
{
	// 셰이더 컴파일 하기
	





	float clearColor[4];
	for (int i = 0; i < 4; ++i)
		clearColor[i] = 0.0f;
	Texture brdflut(
		TextureUsage::RenderTarget,
		CD3DX12_RESOURCE_DESC::Tex2D(ms_Format, m_Width, m_Height, 1,1,1,0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET),
		&CD3DX12_CLEAR_VALUE(ms_Format, clearColor),
		"IntegratedBRDFTexture");




	m_Viewport.Set(m_Width, m_Height);
	m_ScissorRect.Set(m_Width, m_Height);
}