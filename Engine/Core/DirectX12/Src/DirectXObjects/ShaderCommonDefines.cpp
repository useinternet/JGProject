#include<PCH.h>
#include"ShaderCommonDefines.h"
#include"DirectXToolKit/SamplerState.h"
#include"DirectXToolKit/Shader.h"
using namespace Dx12;
using namespace std;


Dx12CommonShaderDefines::Dx12CommonShaderDefines()
{
	m_MainRootSignature.InitAsShaderResourceView(0, 0);
	m_MainRootSignature.InitAsConstantBufferView(0, 0);
	m_MainRootSignature.InitAsShaderResourceView(0, 1);
	m_MainRootSignature.InitAsConstantBufferView(1, 0);
	m_MainRootSignature.InitAsDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 0, 2);
	m_MainRootSignature.InitAsDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 0, 3);
	m_MainRootSignature.AddSampler(SamplerState::PointWrap(0));
	m_MainRootSignature.AddSampler(SamplerState::PointClamp(1));
	m_MainRootSignature.AddSampler(SamplerState::LinearWrap(2));
	m_MainRootSignature.AddSampler(SamplerState::LinearClamp(3));
	m_MainRootSignature.AddSampler(SamplerState::AnisotropicWrap(4));
	m_MainRootSignature.AddSampler(SamplerState::AnisotropicClamp(5));
	m_MainRootSignature.Finalize();
}


GraphicsPSO Dx12CommonShaderDefines::GetMainPSO(const GraphicsShader& shader)
{
	GraphicsPSO pso;
	pso.SetRenderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D24_UNORM_S8_UINT);
	pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pso.SetRootSig(m_MainRootSignature.Get());
	pso.SetVertexShader(shader);
	pso.SetPixelShader(shader);
	pso.SetInputLayout(JgSkinnedVertex::GetNumInputLayout(), JgSkinnedVertex::GetInputLayoutDesc());
	pso.Finalize();

	return pso;
}