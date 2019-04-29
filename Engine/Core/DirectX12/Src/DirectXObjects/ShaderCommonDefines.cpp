#include<PCH.h>
#include"ShaderCommonDefines.h"
#include"DirectXToolKit/SamplerState.h"
#include"DirectXToolKit/Shader.h"
#include"DirectXToolKit/PSO.h"
#include"DirectXToolKit/DepthStencilState.h"
#include"DirectXToolKit/BlendState.h"
#include"DirectXToolKit/RasterizerState.h"
#include"GBuffer.h"
#include"DirectXToolKit/Camera.h"
using namespace Dx12;
using namespace std;
using namespace Common;
void PassCB::Set(const Camera& cam)
{
	auto view = cam.GetHlslViewMatrix();
	auto proj = cam.GetHlslProjMatrix();
	auto viewproj = cam.GetHlslMatrix();


	ViewProj = viewproj.Get();
	InvViewProj = Inverse(viewproj).Get();

	View = view.Get(); 
	InvView = Inverse(view).Get();


	Proj = proj.Get();
	InvProj = Inverse(proj).Get();


	ToEye = cam.GetPosition().Get();
	FarZ  = cam.GetFarZ();
	NearZ = cam.GetNearZ();
}
Dx12CommonShaderDefines::Dx12CommonShaderDefines()
{
	m_MainRootSignature.InitAsShaderResourceView(0, 0); // ObjectCB
	m_MainRootSignature.InitAsConstantBufferView(0, 0); // PassCB
	m_MainRootSignature.InitAsShaderResourceView(0, 1); // MaterialCB
	m_MainRootSignature.InitAsConstantBufferView(1, 0); // SkinnedCB
	m_MainRootSignature.InitAsDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 0, 2); // Texture
	m_MainRootSignature.InitAsDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0, 3); // CubeMap
	//
	// SamplerState
	m_MainRootSignature.AddSampler(SamplerState::PointWrap(0));
	m_MainRootSignature.AddSampler(SamplerState::PointClamp(1));
	m_MainRootSignature.AddSampler(SamplerState::LinearWrap(2));
	m_MainRootSignature.AddSampler(SamplerState::LinearClamp(3));
	m_MainRootSignature.AddSampler(SamplerState::AnisotropicWrap(4));
	m_MainRootSignature.AddSampler(SamplerState::AnisotropicClamp(5));

	m_MainRootSignature.Finalize();
}
GraphicsPSO Dx12CommonShaderDefines::GetPSO(EPreparedPSO pso, const GraphicsShader& shader)
{
	switch (pso)
	{
	case PreparedPSO::Main_Static:
		return GetMainPSO(shader, false);
	case PreparedPSO::Main_Skeletal:
		return GetMainPSO(shader, true);
	case PreparedPSO::SkyBox:
		return GetSkyBoxPSO(shader);
	case PreparedPSO::Scene:
		return GetScenePSO(shader);
	}
	return GraphicsPSO();
}
GraphicsPSO Dx12CommonShaderDefines::GetMainPSO(const GraphicsShader& shader, bool is_skinned)
{
	GraphicsPSO pso;
	pso.SetRenderTargetFormats(GBufferTexture::NumBuffer, GBuffer::GetRtvFormatArray().data(),
		DXGI_FORMAT_D24_UNORM_S8_UINT);

	pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pso.SetRootSig(m_MainRootSignature.Get());
	pso.SetVertexShader(shader);
	pso.SetPixelShader(shader);

	(is_skinned) ?
		pso.SetInputLayout(JgSkinnedVertex::GetNumInputLayout(), JgSkinnedVertex::GetInputLayoutDesc()) :
		pso.SetInputLayout(JgVertex::GetNumInputLayout(), JgVertex::GetInputLayoutDesc());

	pso.Finalize();

	return pso;
}


GraphicsPSO Dx12CommonShaderDefines::GetSkyBoxPSO(const GraphicsShader& shader)
{
	GraphicsPSO pso;
	pso.SetRasterizerState(RasterizerState::CullNone());
	pso.SetDepthStencilState(DepthStencilState::LessEqual());
	pso.SetRenderTargetFormats(GBufferTexture::NumBuffer, GBuffer::GetRtvFormatArray().data(),
		DXGI_FORMAT_D24_UNORM_S8_UINT);
	pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pso.SetRootSig(m_MainRootSignature.Get());
	pso.SetVertexShader(shader);
	pso.SetPixelShader(shader);
	pso.SetInputLayout(JgVertex::GetNumInputLayout(), JgVertex::GetInputLayoutDesc());
	pso.Finalize();

	return pso;
}

GraphicsPSO Dx12CommonShaderDefines::GetScenePSO(const GraphicsShader& shader)
{
	GraphicsPSO pso;
	pso.SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT);
	pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pso.SetRootSig(m_MainRootSignature.Get());
	pso.SetVertexShader(shader);
	pso.SetPixelShader(shader);
	pso.SetInputLayout(JgVertex::GetNumInputLayout(), JgVertex::GetInputLayoutDesc());
	pso.Finalize();

	return pso;
}