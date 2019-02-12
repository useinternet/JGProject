#include"Shader.h"
#include"DxCore/DxDevice.h"
#include"Data/CommonData.h"
using namespace std;
using namespace JGRC;
using namespace Microsoft::WRL;
unordered_map<pair<UINT, wstring>, CommonPSOPack::CommonPSOTuple, pair_hash> CommonPSOPack::m_ShaderPSOList;
UINT CommonPSOPack::PSOCount = 0;
Shader::Shader(const wstring& shaderPath, const ShaderTypeInformation& ShaderTypes)
{
	m_ShaderPath = shaderPath;
	m_ShaderTypeInfor = move(ShaderTypes);
}
void Shader::SetPath(const wstring& shaderPath, const ShaderTypeInformation& ShaderTypes)
{
	m_ShaderPath = shaderPath;
	m_ShaderTypeInfor = move(ShaderTypes);
}
void Shader::Macro_Push(const string& define, UINT value)
{
	m_Macro.push_back(ShaderMacroPack{ define, to_string(value) });
}
void Shader::Macro_Merge(const vector<ShaderMacroPack>& v)
{
	m_Macro.insert(m_Macro.end(), v.begin(), v.end());
}
ComPtr<ID3D12PipelineState> Shader::CompileAndConstrutPSO(EPSOMode mode, ShaderRootSignatureBase* RootSig)
{
	m_ShaderBtCodes.clear();
	Compile(RootSig);
	// 모드 목록 작성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC Desc;
	CreateModeDesc(mode, Desc);

	// 파이프 라인 만들기
	ComPtr<ID3D12PipelineState> pso;
	if (RootSig->GetInputLayoutData() != nullptr)
	{
		Desc.InputLayout = { RootSig->GetInputLayoutData(), RootSig->GetInputLayoutSize() };
	}
	Desc.pRootSignature = RootSig->Get();

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
	ThrowIfFailed(CommonData::_DxDevice()->Get()->CreateGraphicsPipelineState(
		&Desc,
		IID_PPV_ARGS(pso.GetAddressOf())));

	return pso;
}
void Shader::Compile(ShaderRootSignatureBase* RootSig)
{
	std::vector<D3D_SHADER_MACRO> mcro(m_Macro.size());
	
	for (UINT i = 0; i < mcro.size(); ++i)
	{
		mcro[i].Name = m_Macro[i].Macro.c_str();
		mcro[i].Definition = m_Macro[i].Value.c_str();
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
	case EPSOMode::INSTANCE:
	case EPSOMode::DEFAULT:
	case EPSOMode::DEBUG:
		Desc = PipeLineStateSetDesc().SceneDataRenderTarget();
		Desc.RasterizerState = RSDescList::Default();
		Desc.BlendState = BSDescList::Default();
		Desc.DepthStencilState = DSSDescList::Default();
		break;
	case EPSOMode::ONETARGET_DEFAULT:
		Desc = PipeLineStateSetDesc().Get();
		Desc.RasterizerState = RSDescList::Default();
		Desc.BlendState = BSDescList::Default();
		Desc.DepthStencilState = DSSDescList::Default();
		break;
	case EPSOMode::SCENE:
		Desc = PipeLineStateSetDesc().Get();
		Desc.RasterizerState = RSDescList::Default();
		Desc.BlendState = BSDescList::Default();
		Desc.DepthStencilState = DSSDescList::Default();
		break;
	case EPSOMode::SKYBOX:
		Desc = PipeLineStateSetDesc().SceneDataRenderTarget();
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
CommonPSOPack::CommonPSOPack()
{
	PSOCount++;
}
CommonPSOPack::~CommonPSOPack()
{
	PSOCount--;
	if (PSOCount == 0)
	{
		for (auto& list : m_ShaderPSOList)
		{
			list.second.CustomPSO.Reset();
			list.second.ShadowPSO.Reset();
			list.second.ViewNormalPSO.Reset();
		}
	}
}
void CommonPSOPack::CompilePSO(const wstring& path, EPSOMode CustomMode, ShaderRootSignatureBase* RootSig, EShaderFlag flag)
{
	pair<UINT, wstring> key = make_pair(flag, path);
	if (m_ShaderPSOList.find(key) != m_ShaderPSOList.end())
	{
		CustomPSO     = m_ShaderPSOList[key].CustomPSO.Get();
		ShadowPSO     = m_ShaderPSOList[key].ShadowPSO.Get();
		ViewNormalPSO = m_ShaderPSOList[key].ViewNormalPSO.Get();
		return;
	}
	CustomCompiler.SetPath(path, { EShaderType::Vertex, EShaderType::Pixel });
	ShadowCompiler.SetPath(global_shadow_hlsl_path, { EShaderType::Vertex, EShaderType::Pixel });
	ViewNormalCompiler.SetPath(global_drawnormal_hlsl_path, { EShaderType::Vertex, EShaderType::Pixel });


	if (flag & Shader_Flag_Use_DiffuseMap)
	{
		CustomCompiler.Macro_Push(SHADER_MACRO_DEFINE_USING_DIFFUSEMAP, SHADER_MACRO_ONLY_DEFINE);
		ShadowCompiler.Macro_Push(SHADER_MACRO_DEFINE_USING_DIFFUSEMAP, SHADER_MACRO_ONLY_DEFINE);
		ViewNormalCompiler.Macro_Push(SHADER_MACRO_DEFINE_USING_DIFFUSEMAP, SHADER_MACRO_ONLY_DEFINE);
	}
	if (flag & Shader_Flag_Use_NormalMap)
	{
		CustomCompiler.Macro_Push(SHADER_MACRO_DEFINE_USING_NORMALMAP, SHADER_MACRO_ONLY_DEFINE);
		ShadowCompiler.Macro_Push(SHADER_MACRO_DEFINE_USING_NORMALMAP, SHADER_MACRO_ONLY_DEFINE);
		ViewNormalCompiler.Macro_Push(SHADER_MACRO_DEFINE_USING_NORMALMAP, SHADER_MACRO_ONLY_DEFINE);
	}
	if (flag & Shader_Flag_Skinned)
	{
		CustomCompiler.Macro_Push(SHADER_MACRO_DEFINE_SKINNED, SHADER_MACRO_ONLY_DEFINE);
		ShadowCompiler.Macro_Push(SHADER_MACRO_DEFINE_SKINNED, SHADER_MACRO_ONLY_DEFINE);
		ViewNormalCompiler.Macro_Push(SHADER_MACRO_DEFINE_SKINNED, SHADER_MACRO_ONLY_DEFINE);
	}
	m_ShaderPSOList[key].CustomPSO = CustomCompiler.CompileAndConstrutPSO(CustomMode, RootSig);
	m_ShaderPSOList[key].ShadowPSO = ShadowCompiler.CompileAndConstrutPSO(EPSOMode::SHADOW, RootSig);
	m_ShaderPSOList[key].ViewNormalPSO = ViewNormalCompiler.CompileAndConstrutPSO(EPSOMode::SSAO_NORMALMAP, RootSig);
	CustomPSO     = m_ShaderPSOList[key].CustomPSO.Get();
	ShadowPSO     = m_ShaderPSOList[key].ShadowPSO.Get();
	ViewNormalPSO = m_ShaderPSOList[key].ViewNormalPSO.Get();
}
void ComputeShader::Init(DxDevice* Core, ShaderRootSignatureBase* RootSig, const wstring& shaderPath,
	const vector<string>& functionName)
{
	m_DxDevice = Core;
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
	Desc.pRootSignature = RootSig->Get();
	Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	for (auto& shader : m_Shaders)
	{
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
		Desc.CS =
		{
			reinterpret_cast<BYTE*>(shader.second->GetBufferPointer()),
			shader.second->GetBufferSize()
		};
		ThrowIfFailed(m_DxDevice->Get()->CreateComputePipelineState(
			&Desc, IID_PPV_ARGS(pso.GetAddressOf())));
		m_PSOList[shader.first] = move(pso);
	}
}