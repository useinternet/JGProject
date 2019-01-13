#include"Shader.h"
#include"DxCore/DxCore.h"
#include"Data/CommonData.h"
using namespace std;
using namespace JGRC;
using namespace Microsoft::WRL;

void Shader::Init(const wstring& shaderPath, const ShaderTypeInformation& ShaderTypes)
{
	m_ShaderPath = shaderPath;
	m_ShaderTypeInfor = move(ShaderTypes);
}
ID3D12PipelineState* Shader::CompileAndConstrutPSO(EPSOMode mode, ShaderRootSignatureBase* RootSig,
	const vector<ShaderMacroPack>& Macros)
{
	m_ShaderBtCodes.clear();
	Compile(RootSig, Macros);
	// 모드 목록 작성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc;
	CreateModeDesc(mode, Desc);

	// 파이프 라인 만들기
	ComPtr<ID3D12PipelineState> pso;
	if (RootSig->GetInputLayoutData() != nullptr)
	{
		Desc.InputLayout = { RootSig->GetInputLayoutData(), RootSig->GetInputLayoutSize() };
	}
	Desc.pRootSignature = RootSig->GetRootSignature();

	for (auto& shader : m_ShaderBtCodes)
	{
		switch (shader.first)
		{
		case EShaderType::Pixel:
			Desc.PS =
			{
				reinterpret_cast<BYTE*>(shader.second->GetBufferPointer()),
				shader.second->GetBufferSize()
			};
			break;
		case EShaderType::Vertex:
			Desc.VS =
			{
				reinterpret_cast<BYTE*>(shader.second->GetBufferPointer()),
				shader.second->GetBufferSize()
			};
			break;
		case EShaderType::Geometry:
			Desc.GS =
			{
				reinterpret_cast<BYTE*>(shader.second->GetBufferPointer()),
				shader.second->GetBufferSize()
			};
			break;
		case EShaderType::Hull:
			Desc.HS =
			{
				reinterpret_cast<BYTE*>(shader.second->GetBufferPointer()),
				shader.second->GetBufferSize()
			};
			break;
		case EShaderType::Domain:
			Desc.DS =
			{
				reinterpret_cast<BYTE*>(shader.second->GetBufferPointer()),
				shader.second->GetBufferSize()
			};
			break;
		}
	}
	ThrowIfFailed(CommonData::_Device()->CreateGraphicsPipelineState(
		&Desc,
		IID_PPV_ARGS(pso.GetAddressOf())));
	ID3D12PipelineState* result = pso.Get();
	m_PSOList.push_back(move(pso));
	return result;
}
void Shader::Compile(ShaderRootSignatureBase* RootSig, const std::vector<ShaderMacroPack>& Macros)
{
	std::vector<D3D_SHADER_MACRO> mcro(Macros.size());
	for (UINT i = 0; i < mcro.size(); ++i)
	{
		mcro[i].Name = Macros[i].Macro.c_str();
		mcro[i].Definition = Macros[i].Value.c_str();
	}
	mcro.push_back(D3D_SHADER_MACRO{ nullptr, nullptr });
	for (auto& type : m_ShaderTypeInfor)
	{
		switch (type)
		{
		case EShaderType::Vertex:
			m_ShaderBtCodes[type] = d3dUtil::CompileShader(m_ShaderPath.c_str(), mcro.data(),
				VS, "vs_" + Version);
			break;
		case EShaderType::Pixel:
			m_ShaderBtCodes[type] = d3dUtil::CompileShader(m_ShaderPath.c_str(), mcro.data(),
				PS, "ps_" + Version);
			break;
		case EShaderType::Geometry:
			m_ShaderBtCodes[type] = d3dUtil::CompileShader(m_ShaderPath.c_str(), mcro.data(),
				GS, "gs_" + Version);
			break;
		case EShaderType::Hull:
			m_ShaderBtCodes[type] = d3dUtil::CompileShader(m_ShaderPath.c_str(), mcro.data(),
				HS, "hs_" + Version);
			break;
		case EShaderType::Domain:
			m_ShaderBtCodes[type] = d3dUtil::CompileShader(m_ShaderPath.c_str(), mcro.data(),
				DS, "ds_" + Version);
			break;
		}
	}
}
void Shader::CreateModeDesc(EPSOMode mode, D3D12_GRAPHICS_PIPELINE_STATE_DESC& Desc)
{
	switch (mode)
	{
	case EPSOMode::DEFAULT:
		Desc = PipeLineStateSetDesc().Get();
		Desc.RasterizerState = RSDescList::Default();
		Desc.BlendState = BSDescList::Default();
		Desc.DepthStencilState = DSSDescList::Default();
		break;
	case EPSOMode::SKYBOX:
		Desc = PipeLineStateSetDesc().Get();
		Desc.RasterizerState = RSDescList::CullNone();
		Desc.BlendState = BSDescList::Default();
		Desc.DepthStencilState = DSSDescList::LessEqual();
		break;
	case EPSOMode::SHADOW:
		Desc = PipeLineStateSetDesc().UnUsedRenderTarget();
		Desc.RasterizerState = RSDescList::ShadowBias();
		Desc.BlendState = BSDescList::Default();
		Desc.DepthStencilState = DSSDescList::Default();
		break;
	case EPSOMode::SSAO_NORMALMAP:
		Desc = PipeLineStateSetDesc().SSAONormalRenderTarget();
		Desc.RasterizerState = RSDescList::Default();
		Desc.BlendState = BSDescList::Default();
		Desc.DepthStencilState = DSSDescList::Default();
		break;
	case EPSOMode::SSAO:
		Desc = PipeLineStateSetDesc().SSAORenderTarget();
		Desc.RasterizerState = RSDescList::Default();
		Desc.BlendState = BSDescList::Default();
		Desc.DepthStencilState = DSSDescList::NoneDepth();
		break;
	}

}



void ComputeShader::Init(DxCore* Core, ShaderRootSignatureBase* RootSig, const wstring& shaderPath,
	const vector<string>& functionName)
{
	m_DxCore = Core;
	m_ShaderPath = move(shaderPath);
	ShaderCompile(functionName);
	CreatePSO(RootSig);
}
ID3D12PipelineState* ComputeShader::GetPSO(const std::string& name)
{
	if (m_PSOList.end() != m_PSOList.find(name))
	{
		return m_PSOList[name].Get();
	}
	return nullptr;
}
void ComputeShader::ShaderCompile(const vector<string>& functionName)
{
	for (auto& name : functionName)
	{
		if (m_Shaders.end() != m_Shaders.find(name))
		{
			// 예외 처리
		}
		m_Shaders[name] = d3dUtil::CompileShader(m_ShaderPath.c_str(),
			nullptr, name, "cs_" + Version);
	}
}
void ComputeShader::CreatePSO(ShaderRootSignatureBase* RootSig)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC Desc = { };
	Desc.pRootSignature = RootSig->GetRootSignature();
	Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	for (auto& shader : m_Shaders)
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
		Desc.CS =
		{
			reinterpret_cast<BYTE*>(shader.second->GetBufferPointer()),
			shader.second->GetBufferSize()
		};
		ThrowIfFailed(m_DxCore->Device()->CreateComputePipelineState(
			&Desc, IID_PPV_ARGS(pso.GetAddressOf())));
		m_PSOList[shader.first] = move(pso);
	}
}