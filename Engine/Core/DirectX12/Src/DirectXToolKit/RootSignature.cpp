#include<PCH.h>
#include"RootSigature.h"
#include"DxUtil.h"
using namespace Dx12;

void RootSignature::InitAsConstantBufferView(UINT registerIndex, UINT registerspace,
	D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param;
	param.InitAsConstantBufferView(registerIndex, registerspace,
		visibility);
	m_RootParameters.push_back(param);

	++m_RootParamOffset;
}
void RootSignature::InitAsConstant(UINT num32bitValue, UINT registerIndex, UINT registerspace,
	D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param;
	param.InitAsConstants(num32bitValue, registerIndex, registerspace,
		visibility);
	m_RootParameters.push_back(param);

	++m_RootParamOffset;
}
void RootSignature::InitAsShaderResourceView(UINT registerIndex, UINT registerspace,
	D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param;
	param.InitAsShaderResourceView(registerIndex, registerspace,
		visibility);
	m_RootParameters.push_back(param);

	++m_RootParamOffset;
}
void RootSignature::InitAsUnorderedAccessView(UINT registerIndex, UINT registerspace,
	D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param;
	param.InitAsUnorderedAccessView(registerIndex, registerspace,
		visibility);
	m_RootParameters.push_back(param);

	++m_RootParamOffset;
}
void RootSignature::InitAsDescriptor(D3D12_DESCRIPTOR_RANGE_TYPE type , 
	UINT numDescriptor, UINT registerIndex, UINT registerspace,
	D3D12_SHADER_VISIBILITY visibility)
{
	CD3DX12_ROOT_PARAMETER param;

	auto range = std::make_unique<CD3DX12_DESCRIPTOR_RANGE>(type, numDescriptor, registerIndex, registerspace);
	CD3DX12_DESCRIPTOR_RANGE* prange = range.get();
	m_DescriptorRanges.push_back(std::move(range));
	param.InitAsDescriptorTable(1, prange, visibility);



	m_RootSigDescriptorTableDatas.push_back({m_RootParamOffset, numDescriptor});
	m_RootParameters.push_back(param);
	++m_RootParamOffset;
}
void RootSignature::AddSampler(const D3D12_STATIC_SAMPLER_DESC& desc)
{
	m_SamplerDescs.push_back(desc);
}
void RootSignature::Finalize()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
		(UINT)m_RootParameters.size(), m_RootParameters.data(),
		(UINT)m_SamplerDescs.size(), m_SamplerDescs.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	m_RootSignature = DxUtil::CreateRootSignature(&rootSigDesc);
}
ID3D12RootSignature* RootSignature::Get() const
{
	return m_RootSignature.Get();
}
