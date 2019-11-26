#include "pch.h"
#include "ShaderModule.h"
#include "ShaderDataType.h"
#include "ShaderData.h"
#include "Object/DxObject/Resource.h"
#include "Object/DxObject/RootSignature.h"
#include "Object/ReObject/RenderTarget.h"
#include "Object/Shader/ShaderLib.h"
#include "Object/DxObject/RenderDevice.h"
#include "Object/DxObject/PipelineState.h"
#include "Object/DxObject/CommandList.h"
#include "Object/ReObject/ReMesh.h"
#include "Object/ReObject/ReMaterial.h"
#include "Object/ReObject/RenderItem.h"
#include "Object/ReObject/ReCamera.h"
// struct 의 변수 이름과 ConstantBuffer의 변수이름을 잇는다.
// ShaderObject에 들어가 있는 정보
/*

1. shaderModule은 셰이더 코드형태로 저장
2. shaderModuleManager로 module을 불러올수있음
3. shaderPassManager로 module순서를 정할수있다.
4. graphicsShaderModule은 renderengine에 존재하는 모든 오브젝트 공유 가능



*/
using namespace std;
namespace RE
{
	ShaderModule::ShaderModule(const std::string& name, EModuleFormat format) : ReObject(name),
		m_ModuleFormat(format)
	{
		m_ModuleDatasByShaderType[ShaderType::Vertex].Compiler = ShaderCompiler(ShaderType::Vertex);
		m_ModuleDatasByShaderType[ShaderType::Hull].Compiler = ShaderCompiler(ShaderType::Hull);
		m_ModuleDatasByShaderType[ShaderType::Domain].Compiler = ShaderCompiler(ShaderType::Domain);
		m_ModuleDatasByShaderType[ShaderType::Geometry].Compiler = ShaderCompiler(ShaderType::Geometry);
		m_ModuleDatasByShaderType[ShaderType::Pixel].Compiler = ShaderCompiler(ShaderType::Pixel);
		m_ModuleDatasByShaderType[ShaderType::Compute].Compiler = ShaderCompiler(ShaderType::Compute);
	}

	SBDConstantBuffer* ShaderModule::AddConstantBuffer(const std::string& name)
	{
		if (m_SBDPool.find(name) != m_SBDPool.end())
			return nullptr;

		shared_ptr<SBDConstantBuffer> result = make_shared<SBDConstantBuffer>(name);
		m_SBDPool[name] = result;
		return result.get();
	}
	SBDStructuredBuffer* ShaderModule::AddStructuredBuffer(const std::string& name, const std::string& strcut_type_name)
	{
		if (m_SBDPool.find(name) != m_SBDPool.end())
			return nullptr;

		shared_ptr<SBDStructuredBuffer> result = make_shared<SBDStructuredBuffer>(name);
		result->BindStruct(strcut_type_name);
		m_SBDPool[name] = result;
		return result.get();
	}
	void ShaderModule::AddSamplerState(const std::string& name, const D3D12_STATIC_SAMPLER_DESC& desc)
	{
		if (m_SSPool.find(name) != m_SSPool.end())
			return;

		m_SSPool[name] = desc;
	}
	SBDTexture2D* ShaderModule::AddTexture2D(const std::string& name)
	{
		if (m_SBDPool.find(name) != m_SBDPool.end())
			return nullptr;
		shared_ptr<SBDTexture2D> result = make_shared<SBDTexture2D>(name);
		m_SBDPool[name] = result;

		return result.get();
	}
	SBDTextureCube* ShaderModule::AddTextureCube(const std::string& name)
	{
		if (m_SBDPool.find(name) != m_SBDPool.end())
			return nullptr;
		shared_ptr<SBDTextureCube> result = make_shared<SBDTextureCube>(name);
		m_SBDPool[name] = result;

		return result.get();
	}


	SBDConstantBuffer* ShaderModule::FindConstantBuffer(const std::string& name)
	{
		if (m_SBDPool.find(name) == m_SBDPool.end())
			return nullptr;

		auto result = m_SBDPool[name];
		if (result->GetType() == JGShader::ConstantBuffer)
		{
			return (SBDConstantBuffer*)result.get();
		}
		else
			return nullptr;
	}
	SBDStructuredBuffer* ShaderModule::FindStructuredBuffer(const std::string& name)
	{
		if (m_SBDPool.find(name) == m_SBDPool.end())
			return nullptr;

		auto result = m_SBDPool[name];
		if (result->GetType() == JGShader::StructuredBuffer)
		{
			return (SBDStructuredBuffer*)result.get();
		}
		else
			return nullptr;
	}
	D3D12_STATIC_SAMPLER_DESC* ShaderModule::FindSamplerState(const std::string& name)
	{
		if (m_SSPool.find(name) == m_SSPool.end())
			return nullptr;
		return &m_SSPool[name];
	}
	SBDTexture2D* ShaderModule::FindTexture2D(const std::string& name)
	{
		if (m_SBDPool.find(name) == m_SBDPool.end())
			return nullptr;

		auto result = m_SBDPool[name];
		if (result->GetType() == JGShader::Texture2D)
		{
			return (SBDTexture2D*)result.get();
		}
		else
			return nullptr;
	}
	SBDTextureCube* ShaderModule::FindTextureCube(const std::string& name)
	{
		if (m_SBDPool.find(name) == m_SBDPool.end())
			return nullptr;

		auto result = m_SBDPool[name];
		if (result->GetType() == JGShader::TextureCube)
		{
			return (SBDTextureCube*)result.get();
		}
		else
			return nullptr;
	}
	void ShaderModule::SetMainCode(ShaderType shader_type, const std::string& code)
	{
		m_ModuleDatasByShaderType[shader_type].MainCode = code;
	}
	void ShaderModule::RemoveSBD(const std::string& name)
	{
		if (m_SBDPool.find(name) == m_SBDPool.end())
			return;
		m_SBDPool.erase(name);
	}
	void ShaderModule::RemoveSS(const std::string& name)
	{
		if (m_SSPool.find(name) == m_SSPool.end())
			return;
		m_SSPool.erase(name);
	}
	const std::string ShaderModule::GetSafeCode(ShaderType type)
	{
		return m_ModuleDatasByShaderType[type].SafeCode;
	}
	bool ShaderModule::Save(const std::string& path)
	{
		std::ofstream fout;
		fout.open(path, ios::binary);
		if (!fout.is_open())
		{
			RE_LOG_ERROR("Failed Save {0}", GetName());
			return false;
		}
		DataSave(fout);
		return true;
	}
	bool ShaderModule::Load(const std::string& path)
	{
		std::ifstream fin;
		fin.open(path, ios::binary);
		if (!fin.is_open())
		{
			RE_LOG_ERROR("Failed Load {0}", GetName());
			return false;
		}
		DataLoad(fin);
		Compile();
		return true;
	}
	void ShaderModule::DataSave(std::ofstream& fout)
	{
		// 이름
		// 모듈 데이터 셰이더 타입 갯수
		// -- 모듈 데이터 셰이더 타입
		// -- 바인딩된 데이터 갯수
		//    -> 바인된된 데이터 이름
		//    -> 바인딩된 데이터
		DataIO::write(fout, EAssetGroup::AssetGroup_RE);
		DataIO::write(fout, EAssetFormat::AssetFormat_RE_ShaderModule);
		DataIO::write(fout, m_ModuleFormat);
		DataIO::write(fout, GetName());

		// sbd
		DataIO::write(fout, m_SBDPool.size());

		for (auto& sbd : m_SBDPool)
		{
			DataIO::write(fout, sbd.first);
			DataIO::write(fout, sbd.second->GetType());
			sbd.second->Save(fout);
		}
		//ss
		DataIO::write(fout,m_SSPool.size());
		for (auto& ss :m_SSPool)
		{
			DataIO::write(fout, ss.first);
			DataIO::write(fout, ss.second);

		}

		// shader
		DataIO::write(fout, m_ModuleDatasByShaderType.size());
		for (auto& data : m_ModuleDatasByShaderType)
		{
			DataIO::write(fout, data.first);


			data.second.Compiler.Save(fout);
			
			DataIO::write(fout, data.second.MainCode);
			DataIO::write(fout, data.second.SafeCode);
			
			DataIO::write(fout, data.second.Params.size());
			for (auto& param : data.second.Params)
			{
				DataIO::write(fout, param);
			}
		}

	}
	void ShaderModule::DataLoad(std::ifstream& fin)
	{
		EAssetGroup asset_group;
		EAssetFormat asset_format;
		DataIO::read(fin, asset_group);
		DataIO::read(fin, asset_format);
		DataIO::read(fin, m_ModuleFormat);

		string module_name;
		DataIO::read(fin, module_name); SetName(module_name);



		//
		// sbd
		size_t sbdPool_size;
		DataIO::read(fin, sbdPool_size);
		for (size_t i = 0; i < sbdPool_size; ++i)
		{
			string data_name;
			JGShader::EShaderBindData d_type;
			DataIO::read(fin, data_name);
			DataIO::read(fin, d_type);
			auto sbd = GetSBDByType(d_type);
			sbd->Load(fin);
			m_SBDPool[data_name] = move(sbd);
		}
		//ss
		size_t ssPool_size;
		DataIO::read(fin, ssPool_size);
		for (size_t i = 0; i < ssPool_size; ++i)
		{
			D3D12_STATIC_SAMPLER_DESC desc;
			string name;
			DataIO::read(fin, name);
			DataIO::read(fin, desc);
			m_SSPool[name] = desc;
		}


		size_t module_data_size;
		DataIO::read(fin, module_data_size);

		for (size_t i = 0; i < module_data_size; ++i)
		{
			ShaderType shader_type;
			DataIO::read(fin, shader_type);

			//compiler
			m_ModuleDatasByShaderType[shader_type].Compiler.Load(fin);


			// Code
			DataIO::read(fin, m_ModuleDatasByShaderType[shader_type].MainCode);
			DataIO::read(fin, m_ModuleDatasByShaderType[shader_type].SafeCode);
	
			// param
			size_t param_size;
			DataIO::read(fin, param_size);
			for (size_t i = 0; i < param_size; ++i)
			{
				JGShader::EShaderParameter p;
				DataIO::read(fin, p);
				m_ModuleDatasByShaderType[shader_type].Params.push_back(p);
			}

		
		}
	}
	uint32_t ShaderModule::GetRootParamIndex(const std::string& sbd_name)
	{
		if (m_RootParamMap.find(sbd_name) == m_RootParamMap.end())
		{
			return 0;
		}
		return m_RootParamMap[sbd_name];
	}
	GraphicsShaderModule::GraphicsShaderModule(const std::string& name, EModuleFormat format) :
		ShaderModule(name, format)
	{
		m_RenderTarget = make_shared<RenderTarget>();
	}

	void GraphicsShaderModule::AddRenderTargetTexture(const std::string& name, DXGI_FORMAT format, uint32_t miplevels)
	{
		Texture t(name);
		D3D12_CLEAR_VALUE value;
		value.Color[0] = 0.0f;
		value.Color[1] = 0.0f;
		value.Color[2] = 0.0f;
		value.Color[3] = 0.0f;
		value.Format = format;
		t.CreateResource(CD3DX12_RESOURCE_DESC::Tex2D(format, m_Width, m_Height, 1, miplevels, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET), &value);
		uint32_t numRT = m_NumRenderTarget;
		m_RenderTarget->BindTexture(m_NumRenderTarget++, t);

		AddOutputEelement(ShaderType::Pixel, JGShader::ToShaderDataType(format), name, "SV_TARGET" + to_string(numRT));
	}
	void GraphicsShaderModule::AddRenderTargetCubeTexture(const std::string& name, DXGI_FORMAT format, uint32_t miplevels)
	{
		Texture t(name);
		D3D12_CLEAR_VALUE value;
		value.Color[0] = 0.0f;
		value.Color[1] = 0.0f;
		value.Color[2] = 0.0f;
		value.Color[3] = 0.0f;
		value.Format = format;
		t.CreateResource(CD3DX12_RESOURCE_DESC::Tex2D(format, m_Width, m_Height,
			6, miplevels, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET), &value);
		uint32_t numRT = m_NumRenderTarget;
		m_RenderTarget->BindTexture(m_NumRenderTarget++, t);

		AddOutputEelement(ShaderType::Pixel, JGShader::ToShaderDataType(format), name, "SV_TARGET" + to_string(numRT));
	}
	void GraphicsShaderModule::AddDepthStencilTexture(const std::string& name, DXGI_FORMAT format, uint32_t miplevels)
	{
		Texture t(name);
		D3D12_CLEAR_VALUE value;
		value.DepthStencil.Depth = 1.0f;
		value.DepthStencil.Stencil = 0;
		value.Format = format;
		t.CreateResource(CD3DX12_RESOURCE_DESC::Tex2D(format, m_Width, m_Height, 1,
			miplevels, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL), &value);
		m_RenderTarget->BindDepthTexture(t);
	}
	void GraphicsShaderModule::AddDepthStencilCubeTexture(const std::string& name, DXGI_FORMAT format, uint32_t miplevels)
	{
		Texture t(name);
		D3D12_CLEAR_VALUE value;
		value.DepthStencil.Depth = 1.0f;
		value.DepthStencil.Stencil = 0;
		value.Format = format;
		t.CreateResource(CD3DX12_RESOURCE_DESC::Tex2D(format, m_Width, m_Height, 6,
			miplevels, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL), &value);
		m_RenderTarget->BindDepthTexture(t);
	}

	void GraphicsShaderModule::AddInputEelement(ShaderType shader_type, JGShader::EShaderData type, const std::string& name, const std::string& semantic)
	{
		m_Input[shader_type].push_back({ type, name, semantic });
	}
	void GraphicsShaderModule::AddOutputEelement(ShaderType shader_type, JGShader::EShaderData type, const std::string& name, const std::string& semantic)
	{
		m_Output[shader_type].push_back({ type, name, semantic });
	}
	void GraphicsShaderModule::AddShaderParameter(JGShader::EShaderParameter param)
	{
		auto shader_type = JGShader::GetShaderTypeByShaderParam(param);
		m_ModuleDatasByShaderType[shader_type].Params.push_back(param);
	}
	void GraphicsShaderModule::SetRenderTargetTextureName(uint32_t slot, const std::string& name)
	{
		Texture* t = m_RenderTarget->GetTexture(slot);
		if (t == nullptr)
		{
			RE_LOG_ERROR("{0} Slot does not exist", slot);
			return;
		}
		

		t->SetName(name);
	}
	void GraphicsShaderModule::SetRenderTargetTextureFormat(uint32_t slot, DXGI_FORMAT format)
	{
		Texture* t = m_RenderTarget->GetTexture(slot);
		if (t == nullptr)
		{
			RE_LOG_ERROR("{0} Slot does not exist", slot);
			return;
		}
	
		auto desc = t->GetDesc();
		desc.Format = format;
		t->SetDesc(desc);
	}
	void GraphicsShaderModule::SetDepthStencilTextureName(const std::string& name)
	{
		Texture* t = m_RenderTarget->GetDepthTexture();
		if (t == nullptr)
		{
			RE_LOG_ERROR("DepthTexture does not exist");
			return;
		}
		t->SetName(name);
	}
	void GraphicsShaderModule::SetDepthStencilTextureFormat(DXGI_FORMAT format)
	{
		Texture* t = m_RenderTarget->GetDepthTexture();
		if (t == nullptr)
		{
			RE_LOG_ERROR("DepthTexture does not exist");
			return;
		}
		auto desc = t->GetDesc();
		desc.Format = format;
		t->SetDesc(desc);
	}

	void GraphicsShaderModule::SetInputElementSementic(
		ShaderType type, const std::string& name, const std::string& sementic)
	{
		uint32_t size = (uint32_t)m_Input.size();
		for (uint32_t i = 0; i < size; ++i)
		{
			if (m_Input[type][i].name == name)
			{
				m_Input[type][i].semantic = sementic;
				break;
			}
		}
	}
	void GraphicsShaderModule::SetInputElementName(
		ShaderType type, const std::string& origin_name, const std::string& name)
	{
		uint32_t size = (uint32_t)m_Input.size();
		for (uint32_t i = 0; i < size; ++i)
		{
			if (m_Input[type][i].name == origin_name)
			{
				m_Input[type][i].name = name;;
				break;
			}
		}
	}
	void GraphicsShaderModule::SetInputElementDataType(
		ShaderType shader_type, const std::string& name, JGShader::EShaderData type)
	{
		uint32_t size = (uint32_t)m_Input.size();
		for (uint32_t i = 0; i < size; ++i)
		{
			if (m_Input[shader_type][i].name == name)
			{
				m_Input[shader_type][i].type = type;;
				break;
			}
		}
	}
	void GraphicsShaderModule::SetOutputElementSementic(
		ShaderType type, const std::string& name, const std::string& sementic)
	{
		uint32_t size = (uint32_t)m_Output.size();
		for (uint32_t i = 0; i < size; ++i)
		{
			if (m_Output[type][i].name == name)
			{
				m_Output[type][i].semantic = sementic;
				break;
			}
		}
	}
	void GraphicsShaderModule::SetOutputElementName(
		ShaderType type, const std::string& origin_name, const std::string& name)
	{
		uint32_t size = (uint32_t)m_Output.size();
		for (uint32_t i = 0; i < size; ++i)
		{
			if (m_Output[type][i].name == origin_name)
			{
				m_Output[type][i].name = name;;
				break;
			}
		}
	}
	void GraphicsShaderModule::SetOutputElementDataType(
		ShaderType shader_type, const std::string& name, JGShader::EShaderData type)
	{
		uint32_t size = (uint32_t)m_Output.size();
		for (uint32_t i = 0; i < size; ++i)
		{
			if (m_Output[shader_type][i].name == name)
			{
				m_Output[shader_type][i].type = type;;
				break;
			}
		}
	}
	const Texture& GraphicsShaderModule::GetRenderTargetTexture(uint32_t slot) const
	{
		return *m_RenderTarget->GetTexture(slot);
	}


	void GraphicsShaderModule::MakePSO(GraphicsPipelineState* pso)
	{

		Compile();
		// 셰이더 바인딩
		for (auto& module_data : m_ModuleDatasByShaderType)
		{
			if (module_data.second._Shader)
			{
				pso->BindShader(*module_data.second._Shader);

			}

		}
	


		// 렌더타겟 바인딩
		pso->BindRenderTarget(*m_RenderTarget);
		// 루트 서명 바인딩
		pso->SetRootSignature(*m_RootSignature);

		std::vector<D3D12_INPUT_ELEMENT_DESC> input;
		uint32_t acc_size = 0;
		for (auto& element : m_Input[ShaderType::Vertex])
		{
			D3D12_INPUT_ELEMENT_DESC desc = {};
			desc = { element.semantic.c_str(), 0, JGShader::ToFormat(element.type), 0, acc_size,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 };

			acc_size += JGShader::GetDataSize(element.type);
			input.push_back(desc);
		}
		pso->SetInputLayout(input);
	}
	struct ShaderRegisterData
	{

		/*
		-- u,t 일 경우

		t 종류 : StructuredBuffer, Texture, TextureCube
		  1. Texture          : RegisterSpcae 0 고정 RegisterNumber 변동
		  2. TextureCube      : RegisterSpace 1 고정 RegisterNumber 변동
		  3. StructuredBuffer : RegisterSpace   변동 RegisterNumber 0 고정


		u 종류 : RWStructuredBuffer, RWTexture
		  1. RWTexture :  RegisterSpace 0 고정 RegisterNumber 변동


		-- ConstantBuffer일 경우
		1. RegisterSpace는 무조건 0 고정
		2. RegisterNumber가 변동

		-- SamplerState일 경우
		1. RegisterSpace는 무조건 0 고정
		2. RegisterNumber가 변동

		*/
		//  RegisterSpace = rs, RegisterNumber = rn 초기 값
		bool is_exist = false;
		// texture
		uint32_t t_rn = 0;
		uint32_t t_rs = 0;

		// textureCube
		uint32_t tc_rn = 0;
		uint32_t tc_rs = 1;

		// StructuredBuffer
		uint32_t sb_rn = 0;
		uint32_t sb_rs = 2;

		// rwtexture
		uint32_t rt_rn = 0;
		uint32_t rt_rs = 0;

		// RWStructuredBuffer
		uint32_t rsb_rn = 0;
		uint32_t rsb_rs = 1;

		// ConstantBuffer
		uint32_t cb_rn = 0;
		uint32_t cb_rs = 0;

		// SamplerState
		uint32_t ss_rn = 0;
		uint32_t ss_rs = 0;
	};
	bool GraphicsShaderModule::MakeRootSignature()
	{
		// RootSignature
		std::map<uint32_t, std::string> sb_rootparamMap;
		std::map<uint32_t, std::string> rsb_rootparamMap;
		std::map<uint32_t, std::string> cb_rootparamMap;
		uint32_t numRootParam = 0;
		ShaderRegisterData register_data;
		numRootParam = 1;
		for (auto& sbdpair : m_SBDPool)
		{
			auto& var_name = sbdpair.first;
			auto& sbd = sbdpair.second;

			auto type = sbd->GetType();
			auto state = sbd->GetState();

			// texture
			if (type == JGShader::EShaderBindData::Texture2D &&
				state == JGShader::EShaderBindDataState::ReadOnly)
			{
				// RegisterSpcae 0 고정 RegisterNumber 변동
				sbd->SetShaderRegister(register_data.t_rn, register_data.t_rs);
				auto sbd_texture2d = (SBDTexture2D*)sbd.get();
				register_data.t_rn += sbd_texture2d->Count();
			}
			// textureCube
			else if (type == JGShader::EShaderBindData::TextureCube)
			{
				//  RegisterSpace 1 고정 RegisterNumber 변동
				sbd->SetShaderRegister(register_data.tc_rn, register_data.tc_rs);
				auto sbd_textureCube = (SBDTextureCube*)sbd.get();
				register_data.tc_rn += sbd_textureCube->Count();
			}
			// structuredBuffer
			else if (type == JGShader::EShaderBindData::StructuredBuffer &&
				state == JGShader::EShaderBindDataState::ReadOnly)
			{
				// RegisterSpace   변동 RegisterNumber 0 고정
				sb_rootparamMap[register_data.sb_rs] = sbd->GetName();
				sbd->SetShaderRegister(register_data.sb_rn, register_data.sb_rs);
				++register_data.sb_rs;
				++numRootParam;
			}
			// rwtexture
			else if (type == JGShader::EShaderBindData::Texture2D &&
				state == JGShader::EShaderBindDataState::ReadWrite)
			{
				// RegisterSpace 0 고정 RegisterNumber 변동
				sbd->SetShaderRegister(register_data.rt_rn, register_data.rt_rs);
				auto sbd_texture2d = (SBDTexture2D*)sbd.get();
				register_data.rt_rn += sbd_texture2d->Count();

			}
			// rwstructuredBuffer
			else if (type == JGShader::EShaderBindData::StructuredBuffer &&
				state == JGShader::EShaderBindDataState::ReadWrite)
			{
				rsb_rootparamMap[register_data.rsb_rs] = sbd->GetName();
				// RegisterSpace   변동 RegisterNumber 0 고정
				sbd->SetShaderRegister(register_data.rsb_rn, register_data.rsb_rs);
				++register_data.rsb_rs;
				++numRootParam;
			}
			else if (type == JGShader::EShaderBindData::ConstantBuffer)
			{
				cb_rootparamMap[register_data.cb_rn] = sbd->GetName();
				// RegisterSpace는 무조건 0 고정  RegisterNumber가 변동
				sbd->SetShaderRegister(register_data.cb_rn, register_data.cb_rs);
				++register_data.cb_rn;
				++numRootParam;
			}
			else
			{
				RE_LOG_ERROR("does not exist ShaderBindDataType");
			}
		}
		for (auto& ss : m_SSPool)
		{
			ss.second.RegisterSpace = register_data.ss_rs;
			ss.second.ShaderRegister = register_data.ss_rn;
			++register_data.ss_rn;
		}



		if (register_data.t_rn == 0 && register_data.tc_rn == 0 && register_data.rt_rn == 0)
			numRootParam--;



		m_RootSignature = make_shared<RootSignature>(numRootParam);
		int param_index = 0;
		// texture
		// textureCube
		// rwtexture
		uint32_t p1_idx = 0;
		if (register_data.t_rn != 0 || register_data.tc_rn != 0 || register_data.rt_rn != 0)
		{
			p1_idx = param_index++;
		}
		if (register_data.t_rn != 0)
		{
			m_RootSignature->InitParam(p1_idx).PushAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				register_data.t_rn, 0, register_data.t_rs);
		}
		if (register_data.tc_rn != 0)
		{
			m_RootSignature->InitParam(p1_idx).PushAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				register_data.tc_rn, 0, register_data.tc_rs);
		}
		if (register_data.rt_rn != 0)
		{
			m_RootSignature->InitParam(p1_idx).PushAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
				register_data.rt_rn, 0, register_data.rt_rs);
		}
		if (register_data.t_rn != 0 || register_data.tc_rn != 0 || register_data.rt_rn != 0)
		{
			m_RootSignature->InitParam(p1_idx).InitAsDescriptorTable();
		}

		m_RootParamMap.clear();
		// structuredBuffer
		for (uint32_t i = 2; i < register_data.sb_rs; ++i)
		{
			auto name = sb_rootparamMap[i];
			m_RootParamMap[name] = param_index;
			m_RootSignature->InitParam(param_index++).InitAsSRV(0, i);

		}
		// rwstructuredBuffer
		for (uint32_t i = 1; i < register_data.rsb_rs; ++i)
		{
			auto name = rsb_rootparamMap[i];
			m_RootParamMap[name] = param_index;
			m_RootSignature->InitParam(param_index++).InitAsUAV(0, i);
		}

		// constbuffer
		for (uint32_t i = 0; i < register_data.cb_rn; ++i)
		{
			auto name = cb_rootparamMap[i];
			m_RootParamMap[name] = param_index;
			m_RootSignature->InitParam(param_index++).InitAsConstantBufferView(i);
		}


		for (auto& ss : m_SSPool)
		{
			m_RootSignature->AddStaticSampler(ss.second);
		}

		if (!m_RootSignature->Finalize())
		{
			return false;
		}

		return true;
	}

	void GraphicsShaderModule::BindCamera(ReCamera* cam)
	{
		m_BindedCamera = cam;
		if (m_Width != cam->GetLensWidth() ||
			m_Height != cam->GetLensHeight())
		{
			m_Width = (uint32_t)cam->GetLensWidth();
			m_Height = (uint32_t)cam->GetLensHeight();

			m_RenderTarget->Resize(m_Width, m_Height);
		}

	}
	
	bool GraphicsShaderModule::Compile()
	{
		bool result = true;
		if (m_RootSignature == nullptr)
		{
			MakeRootSignature();
		}

		if (!result)
			return false;
		// Shader 컴파일
		for (auto& pair : m_ModuleDatasByShaderType)
		{
			//auto& shader_type = pair.first;
			auto& module_data = pair.second;
			if (module_data.MainCode.length() != 0)
			{
				module_data._Shader = Shader::MakeShader(pair.first);
				if (!CustomCompile(module_data._Shader, module_data.MainCode))
				{
					result = false;
				}
			}

	
		}
	
		return result;
	}
	bool GraphicsShaderModule::CustomCompile(std::shared_ptr<Shader>& shader, const std::string& main_code)
	{
		bool result = true;
		if (m_RootSignature == nullptr)
		{
			MakeRootSignature();
		}

		if (!result)
			return false;

		auto shader_type = shader->GetType();
		auto& module_data = m_ModuleDatasByShaderType[shader_type];

		// 입력 출력 요소가 없거나 메인코드가 없다면
		// 컴파일 하지 않는다.
		if (main_code.length() == 0      ||
			m_Input[shader_type].empty() ||
			m_Output[shader_type].empty())
		{
			return false;
		}
		
		auto LibManager = GetShaderLibManager();

		string code;
		code += "// ShaderLibraryCode Part      \n\n\n\n";
		code += LibManager->GetCode();

		code += "\n\n\n\n";
		code += "// ShaderModule Part";
		code += "\n\n\n\n";

		code += GetCode(shader_type, main_code);

		shader->Set(code);
		auto device = GetRenderDevice();
		auto is_success = device->CompileShader(*shader, module_data.Compiler);

		if (is_success)
		{
			module_data.SafeCode = code;
		}
		else
		{
			result = false;
		}
		return result;
	}
	string GraphicsShaderModule::GetCode(ShaderType type, const std::string& main_code)
	{
		string code;
		auto& moduleData = m_ModuleDatasByShaderType[type];

		// 텍스쳐
		// 샘플러
		uint32_t idx = 0;
		for (auto& ss : m_SSPool)
		{
			code += "SamplerState " + ss.first + " : register(s" + to_string(idx++) + ");\n";
		}
		for (auto& sbd : m_SBDPool)
		{
			code += sbd.second->GetCode();
		}

		// input
		code += "struct Input \n{ \n";
		for (auto& io : m_Input[type])
		{
			code += "   " + JGShader::ToString(io.type) + " " + io.name + " : " + io.semantic + ";\n";
		}
		code += "};\n";
		// output
		code += "struct Output \n{ \n";
		for (auto& io : m_Output[type])
		{
			code += "   " + JGShader::ToString(io.type) + " " + io.name + " : " + io.semantic + ";\n";
		}
		code += "};\n";





		code += "Output " + moduleData.Compiler.EntryPoint + "(Input input";


		for (auto& param : moduleData.Params)
		{
			code += ", " + JGShader::ToString(param);
		}
		code += ")\n{\n";


		code += main_code;

		code += "};\n";
		return code;
	}


	void GraphicsShaderModule::Execute(CommandList* cmdList)
	{

	}
	void GraphicsShaderModule::DataSave(std::ofstream& fout)
	{
		ShaderModule::DataSave(fout);
		
		DataIO::write(fout, m_Width);
		DataIO::write(fout, m_Height);

		// 텍스쳐 
		DataIO::write(fout, m_NumRenderTarget);
		for (uint32_t i = 0; i < m_NumRenderTarget; ++i)
		{
			auto t = m_RenderTarget->GetTexture(i);
			DataIO::write(fout, t->GetName());
			DataIO::write(fout, t->GetDesc().MipLevels);
			DataIO::write(fout, t->GetDesc().DepthOrArraySize);
			DataIO::write(fout, t->GetDesc().Format);
		}
		auto depth = m_RenderTarget->GetDepthTexture();
		DataIO::write(fout, depth->GetName());
		DataIO::write(fout, depth->GetDesc().MipLevels);
		DataIO::write(fout, depth->GetDesc().DepthOrArraySize);
		DataIO::write(fout, depth->GetDesc().Format);

		// input
		DataIO::write(fout, m_Input.size());
		for (auto& input : m_Input)
		{
			DataIO::write(fout, input.first);
			DataIO::write(fout, input.second.size());

			for (auto& e : input.second)
			{
				DataIO::write(fout, e.type);
				DataIO::write(fout, e.name);
				DataIO::write(fout, e.semantic);
			}
		}

		// output
		DataIO::write(fout, m_Output.size());
		for (auto& output : m_Output)
		{
			DataIO::write(fout, output.first);
			DataIO::write(fout, output.second.size());

			for (auto& e : output.second)
			{
				DataIO::write(fout, e.type);
				DataIO::write(fout, e.name);
				DataIO::write(fout, e.semantic);
			}
		}

	}

	void GraphicsShaderModule::DataLoad(std::ifstream& fin)
	{
		ShaderModule::DataLoad(fin);

		DataIO::read(fin, m_Width);
		DataIO::read(fin, m_Height);
		// texture
		uint32_t numRt = 0;
		DataIO::read(fin, numRt);
		for (uint32_t i = 0; i < numRt; ++i)
		{
			string name;
			UINT16 array_size;
			UINT16 miplevels;
			DXGI_FORMAT format;
			DataIO::read(fin, name);
			DataIO::read(fin, miplevels);
			DataIO::read(fin, array_size);
			DataIO::read(fin, format);

			if (array_size == 6)
			{
				AddRenderTargetCubeTexture(name, format, miplevels);
			}
			else
			{
				AddRenderTargetTexture(name, format, miplevels);
			}
		
		}
		string name;
		UINT16 array_size;
		UINT16 miplevels;
		DXGI_FORMAT format;
		DataIO::read(fin, name);
		DataIO::read(fin, miplevels);
		DataIO::read(fin, array_size);
		DataIO::read(fin, format);
		if (array_size == 6)
		{
			AddDepthStencilCubeTexture(name, format, miplevels);
		}
		else
		{
			AddDepthStencilTexture(name, format, miplevels);
		}
		// input
		size_t input_size = 0;
		DataIO::read(fin, input_size);
		for (size_t i = 0; i < input_size; ++i)
		{
			ShaderType shader_type;
			size_t input_element_size;
			DataIO::read(fin, shader_type);
			DataIO::read(fin, input_element_size);

			for (size_t j = 0; j < input_element_size; ++j)
			{
				InputOutput e;
				DataIO::read(fin, e.type);
				DataIO::read(fin, e.name);
				DataIO::read(fin, e.semantic);
				m_Input[shader_type].push_back(e);
			}
		}


		// output
		size_t output_size = 0;
		DataIO::read(fin, output_size);
		for (size_t i = 0; i < output_size; ++i)
		{
			ShaderType shader_type;
			size_t output_element_size;
			DataIO::read(fin, shader_type);
			DataIO::read(fin, output_element_size);

			for (size_t j = 0; j < output_element_size; ++j)
			{
				InputOutput e;
				DataIO::read(fin, e.type);
				DataIO::read(fin, e.name);
				DataIO::read(fin, e.semantic);
				if (shader_type == ShaderType::Pixel)
					continue;
				m_Output[shader_type].push_back(e);
			}
		}
	}
	string EntryShaderModule::GameObjectStructuredBufferName = "GameObjectArray";
	string EntryShaderModule::CameraConstantBufferName       = "CameraCB";
	string EntryShaderModule::MaterialTextureArrayName       = "MaterialTextures";
	string EntryShaderModule::MaterialConstantBufferName     = "MaterialCB";
	const std::string& EntryShaderModule::GameObjectSBName() {
		return GameObjectStructuredBufferName;
	}
	const std::string& EntryShaderModule::CameraCBName() {
		return CameraConstantBufferName;
	}
	const std::string& EntryShaderModule::MatTextureArrayName() {
		return MaterialTextureArrayName;
	}
	const std::string& EntryShaderModule::MatCBName() {
		return MaterialConstantBufferName;
	}
	//const std::string& EntryShaderModule::BindedGameObjectStructName()
	//{
	//	return GameObjectStructNameToBind;
	//}
	//const std::string& EntryShaderModule::BindedCameraStructName();
	bool EntryShaderModule::Load(const std::string& path)
	{
		if (GraphicsShaderModule::Load(path))
			return true;
		Init();


		return true;
	}

	void EntryShaderModule::Init()
	{
		AddStructuredBuffer(GameObjectSBName(), GameObjectStructNameToBind);
		auto cbuffer = AddConstantBuffer(CameraCBName());
		cbuffer->Add(CameraStructNameToBind, "camera");



		//AddSamplerState("")
		AddTexture2D(MatTextureArrayName())->Resize(100);
		AddSamplerState("AnisotropicSampler", CD3DX12_STATIC_SAMPLER_DESC(0));
	}

	void EntryShaderModule::BindCamera(ReCamera* cam)
	{
		GraphicsShaderModule::BindCamera(cam);
		if (m_BindedCamera)
		{
			auto cbuffer = FindConstantBuffer(CameraCBName());
			auto camera = ((STStruct*)cbuffer->Get("camera"));

			JMatrix view = JMatrix::Transpose(m_BindedCamera->GetView());
			JMatrix proj = JMatrix::Transpose(m_BindedCamera->GetProj());
			JMatrix viewproj = JMatrix::Transpose(m_BindedCamera->GetViewProj());


			((STMatrix*)camera->GetElement("View"))->Set(view);
			((STMatrix*)camera->GetElement("Proj"))->Set(proj);
			((STMatrix*)camera->GetElement("ViewProj"))->Set(viewproj);
			((STMatrix*)camera->GetElement("InvView"))->Set(JMatrix::Inverse(view));
			((STMatrix*)camera->GetElement("InvProj"))->Set(JMatrix::Inverse(proj));
			((STMatrix*)camera->GetElement("InvViewProj"))->Set(JMatrix::Inverse(viewproj));
			((STFloat3*)camera->GetElement("Position"))->Set(m_BindedCamera->GetPosition());
			((STFloat*)camera->GetElement("FarZ"))->Set(m_BindedCamera->GetFarZ());
			((STFloat*)camera->GetElement("NearZ"))->Set(m_BindedCamera->GetNearZ());
			((STFloat2*)camera->GetElement("ScreenSize"))->Set(
				{ m_BindedCamera->GetLensWidth(), m_BindedCamera->GetLensHeight() });

		}
	}

	void EntryShaderModule::Execute(CommandList* cmdList)
	{
	}




	StaticGBufferModule::StaticGBufferModule(const std::string& name) : 
		EntryShaderModule(name, EModuleFormat::G_StaticGBuffer)
	{
	
	}
	void StaticGBufferModule::Init()
	{
		EntryShaderModule::Init();
		// Vertex //
		// 		
		// INPUT
		AddInputEelement(ShaderType::Vertex, JGShader::_float3, "Position", "POSITION");
		AddInputEelement(ShaderType::Vertex, JGShader::_float3, "Normal", "NORMAL");
		AddInputEelement(ShaderType::Vertex, JGShader::_float3, "Tangent", "TANGENT");
		AddInputEelement(ShaderType::Vertex, JGShader::_float2, "TexC", "TEXCOORD");
		// OUTPUT
		AddOutputEelement(ShaderType::Vertex, JGShader::_float4, "PosH", "SV_POSITION");
		AddOutputEelement(ShaderType::Vertex, JGShader::_float3, "PosW", "POSITION");
		AddOutputEelement(ShaderType::Vertex, JGShader::_float3, "NormalW", "NORMAL");
		AddOutputEelement(ShaderType::Vertex, JGShader::_float3, "TangentW", "TANGENT");
		AddOutputEelement(ShaderType::Vertex, JGShader::_float2, "TexC", "TEXCOORD");
		AddOutputEelement(ShaderType::Vertex, JGShader::_float, "Depth", "DEPTH");
		AddOutputEelement(ShaderType::Vertex, JGShader::_uint, "InstanceID", "INSTANCE");
		// Param
		AddShaderParameter(JGShader::Vertex_InstanceID);
		// MainCode




		SetMainCode(ShaderType::Vertex,
			R"(

    Output output;

    GameObject obj = GameObjectArray[instanceID];
    float4x4 world = obj.World;



    float4 posW = mul(float4(input.Position, 1.0f), world);

    output.PosH = mul(posW, camera.ViewProj);
    output.PosW = posW.xyz;
    output.NormalW = normalize(mul(input.Normal, (float3x3)world));
    output.TangentW = normalize(mul(input.Tangent, (float3x3)world));
    output.Depth = output.PosH.z / output.PosH.w;
    output.TexC = input.TexC;
    output.InstanceID = instanceID;


    return output;

)");

		AddInputEelement(ShaderType::Pixel, JGShader::_float4, "PosH", "SV_POSITION");
		AddInputEelement(ShaderType::Pixel, JGShader::_float3, "PosW", "POSITION");
		AddInputEelement(ShaderType::Pixel, JGShader::_float3, "NormalW", "NORMAL");
		AddInputEelement(ShaderType::Pixel, JGShader::_float3, "TangentW", "TANGENT");
		AddInputEelement(ShaderType::Pixel, JGShader::_float2, "TexC", "TEXCOORD");
		AddInputEelement(ShaderType::Pixel, JGShader::_float, "Depth", "DEPTH");
		AddInputEelement(ShaderType::Pixel, JGShader::_uint, "InstanceID", "INSTANCE");



		AddRenderTargetTexture("Default", DXGI_FORMAT_R8G8B8A8_UNORM, 1);
		AddDepthStencilTexture("DefaultDepth", DXGI_FORMAT_D24_UNORM_S8_UINT, 1);
		//AddRenderTargetTexture("BaseColor_Oc", DXGI_FORMAT_R8G8B8A8_UNORM);
		//AddRenderTargetTexture("Normal_AO", DXGI_FORMAT_R8G8B8A8_UNORM);
		//AddRenderTargetTexture("MSR", DXGI_FORMAT_R8G8B8A8_UNORM);
		//AddRenderTargetTexture("EmessiveColor", DXGI_FORMAT_R8G8B8A8_UNORM);
		//AddRenderTargetTexture("Depth", DXGI_FORMAT_R32_FLOAT);
	}
	void StaticGBufferModule::Execute(CommandList* cmdList)
	{
		auto RIManager = GetRenderItemManager();

		Viewport viewport;
		ScissorRect rect;
		viewport.Set((float)m_Width, (float)m_Height);
		rect.Set(m_Width, m_Height);

		cmdList->SetViewport(viewport);
		cmdList->SetScissorRect(rect);
		cmdList->SetGraphicsRootSignature(*m_RootSignature);
		cmdList->ClearRenderTarget(*m_RenderTarget);
		cmdList->SetRenderTarget(*m_RenderTarget);


		// Camera
		if (m_BindedCamera)
		{
			auto cbuffer = FindConstantBuffer(CameraCBName());
			cmdList->BindGraphicsDynamicConstantBuffer(
				GetRootParamIndex(CameraCBName()), cbuffer->GetData());
		}



		auto item_array = RIManager->GetItemByMesh(EReMeshType::Static);

		// GameObjectArray
		auto sbuffer = FindStructuredBuffer(GameObjectSBName());
		auto clone = sbuffer->CloneBindedStruct();
		uint32_t sbuffer_element_count = sbuffer->GetElementCount();
		for (uint32_t i = 0; i < item_array.size(); ++i)
		{
			RenderItem* item = item_array[i];
			if (clone.GetSize() != item->StructuredBuffer->CloneBindedStruct().GetSize())
				continue;

			cmdList->BindGraphicsDynamicStructuredBuffer(
				GetRootParamIndex(GameObjectSBName()),
				clone.GetSize(), 
				item->StructuredBuffer->GetData());


			auto t = item->Material->GetBindedTextures();
			
			if (!t.empty())
			{
				cmdList->BindSRV(GetRootParamIndex(MatTextureArrayName()), t);
			}
	
			cmdList->SetPipelineState(*item->Material->GetPSO());
			item->Mesh->Draw(cmdList, (uint32_t)item->InstanceItems.size());
		}

	}

	SkeletalGBufferModule::SkeletalGBufferModule(const std::string& name) :
		EntryShaderModule(name, EModuleFormat::G_SkeletalGBuffer)
	{

	}

	GUIModule::GUIModule(const std::string& name)
	{
	}

	void GUIModule::Init()
	{
	}

	void GUIModule::BindCamera(ReCamera* cam)
	{
	}

	void GUIModule::Execute(CommandList* cmdList)
	{
	}

}




