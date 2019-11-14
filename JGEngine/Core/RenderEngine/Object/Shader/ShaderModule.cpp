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
	ShaderModule::ShaderModule(const std::string& name) : ReObject(name)
	{
		m_ModuleDatasByShaderType[ShaderType::Vertex].Compiler = ShaderCompiler(ShaderType::Vertex);
		m_ModuleDatasByShaderType[ShaderType::Hull].Compiler = ShaderCompiler(ShaderType::Hull);
		m_ModuleDatasByShaderType[ShaderType::Domain].Compiler = ShaderCompiler(ShaderType::Domain);
		m_ModuleDatasByShaderType[ShaderType::Geometry].Compiler = ShaderCompiler(ShaderType::Geometry);
		m_ModuleDatasByShaderType[ShaderType::Pixel].Compiler = ShaderCompiler(ShaderType::Pixel);
		m_ModuleDatasByShaderType[ShaderType::Compute].Compiler = ShaderCompiler(ShaderType::Compute);
	}

	SBDConstantBuffer* ShaderModule::AddConstantBuffer(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SBDPool.find(name) != m_ModuleDatasByShaderType[type].SBDPool.end())
			return nullptr;

		shared_ptr<SBDConstantBuffer> result = make_shared<SBDConstantBuffer>(name);
		m_ModuleDatasByShaderType[type].SBDPool[name] = result;
		return result.get();
	}
	SBDStructuredBuffer* ShaderModule::AddStructuredBuffer(const std::string& name, const std::string& strcut_type_name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SBDPool.find(name) != m_ModuleDatasByShaderType[type].SBDPool.end())
			return nullptr;

		shared_ptr<SBDStructuredBuffer> result = make_shared<SBDStructuredBuffer>(name);
		result->BindStruct(strcut_type_name);
		m_ModuleDatasByShaderType[type].SBDPool[name] = result;
		return result.get();
	}
	void ShaderModule::AddSamplerState(const std::string& name, const D3D12_STATIC_SAMPLER_DESC& desc, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SSPool.find(name) != m_ModuleDatasByShaderType[type].SSPool.end())
			return;

		m_ModuleDatasByShaderType[type].SSPool[name] = desc;
	}
	SBDTexture2D* ShaderModule::AddTexture2D(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SBDPool.find(name) != m_ModuleDatasByShaderType[type].SBDPool.end())
			return nullptr;
		shared_ptr<SBDTexture2D> result = make_shared<SBDTexture2D>(name);
		m_ModuleDatasByShaderType[type].SBDPool[name] = result;

		return result.get();
	}
	SBDTextureCube* ShaderModule::AddTextureCube(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SBDPool.find(name) != m_ModuleDatasByShaderType[type].SBDPool.end())
			return nullptr;
		shared_ptr<SBDTextureCube> result = make_shared<SBDTextureCube>(name);
		m_ModuleDatasByShaderType[type].SBDPool[name] = result;

		return result.get();
	}


	SBDConstantBuffer* ShaderModule::FindConstantBuffer(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SBDPool.find(name) == m_ModuleDatasByShaderType[type].SBDPool.end())
			return nullptr;

		auto result = m_ModuleDatasByShaderType[type].SBDPool[name];
		if (result->GetType() == JGShader::ConstantBuffer)
		{
			return (SBDConstantBuffer*)result.get();
		}
		else
			return nullptr;
	}
	SBDStructuredBuffer* ShaderModule::FindStructuredBuffer(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SBDPool.find(name) == m_ModuleDatasByShaderType[type].SBDPool.end())
			return nullptr;

		auto result = m_ModuleDatasByShaderType[type].SBDPool[name];
		if (result->GetType() == JGShader::StructuredBuffer)
		{
			return (SBDStructuredBuffer*)result.get();
		}
		else
			return nullptr;
	}
	D3D12_STATIC_SAMPLER_DESC* ShaderModule::FindSamplerState(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SSPool.find(name) == m_ModuleDatasByShaderType[type].SSPool.end())
			return nullptr;
		return &m_ModuleDatasByShaderType[type].SSPool[name];
	}
	SBDTexture2D* ShaderModule::FindTexture2D(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SBDPool.find(name) == m_ModuleDatasByShaderType[type].SBDPool.end())
			return nullptr;

		auto result = m_ModuleDatasByShaderType[type].SBDPool[name];
		if (result->GetType() == JGShader::Texture2D)
		{
			return (SBDTexture2D*)result.get();
		}
		else
			return nullptr;
	}
	SBDTextureCube* ShaderModule::FindTextureCube(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SBDPool.find(name) == m_ModuleDatasByShaderType[type].SBDPool.end())
			return nullptr;

		auto result = m_ModuleDatasByShaderType[type].SBDPool[name];
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
	void ShaderModule::RemoveSBD(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SBDPool.find(name) == m_ModuleDatasByShaderType[type].SBDPool.end())
			return;
		m_ModuleDatasByShaderType[type].SBDPool.erase(name);
	}
	void ShaderModule::RemoveSS(const std::string& name, ShaderType type)
	{
		if (m_ModuleDatasByShaderType[type].SSPool.find(name) == m_ModuleDatasByShaderType[type].SSPool.end())
			return;
		m_ModuleDatasByShaderType[type].SSPool.erase(name);
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
		 
		DataIO::write(fout, GetName());
		DataIO::write(fout, m_ModuleDatasByShaderType.size());
		for (auto& data : m_ModuleDatasByShaderType)
		{
			DataIO::write(fout, data.first);
			DataIO::write(fout, data.second.SBDPool.size());

			for (auto& sbd : data.second.SBDPool)
			{
				DataIO::write(fout, sbd.first);
				DataIO::write(fout, sbd.second->GetType());
				sbd.second->Save(fout);
			}

			data.second.Compiler.Save(fout);
			
			DataIO::write(fout, data.second.SSPool.size());
			for (auto& ss : data.second.SSPool)
			{
				DataIO::write(fout, ss.first);
				DataIO::write(fout, ss.second);

			}

		
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

		string module_name;
		DataIO::read(fin, module_name); SetName(module_name);
		size_t module_data_size;
		DataIO::read(fin, module_data_size);

		for (size_t i = 0; i < module_data_size; ++i)
		{
			ShaderType shader_type;
			DataIO::read(fin, shader_type);

			size_t sbdPool_size;
			DataIO::read(fin, sbdPool_size);

			// sbd
			for (size_t i = 0; i < sbdPool_size; ++i)
			{
				string data_name;
				JGShader::EShaderBindData d_type;
				DataIO::read(fin, data_name);
				DataIO::read(fin, d_type);
				auto sbd = GetSBDByType(d_type);
				sbd->Load(fin);
				m_ModuleDatasByShaderType[shader_type].SBDPool[data_name] = move(sbd);
			}

			//compiler
			m_ModuleDatasByShaderType[shader_type].Compiler.Load(fin);

			//ss
			size_t ssPool_size;
			DataIO::read(fin, ssPool_size);
			for (size_t i = 0; i < ssPool_size; ++i)
			{
				D3D12_STATIC_SAMPLER_DESC desc;
				string name;
				DataIO::read(fin, name);
				DataIO::read(fin, desc);
				m_ModuleDatasByShaderType[shader_type].SSPool[name] = desc;
			}

			// Code
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

	GraphicsShaderModule::GraphicsShaderModule(const std::string& name) : ShaderModule(name)
	{
		m_RenderTarget = make_shared<RenderTarget>();
	}

	void GraphicsShaderModule::AddRenderTargetTexture(const std::string& name, DXGI_FORMAT format)
	{
		Texture t(name);
		t.CreateResource(CD3DX12_RESOURCE_DESC::Tex2D(format, m_Width, m_Height, 1, 0, 1, 0,
			D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET));
		uint32_t numRT = m_NumRenderTarget;
		m_RenderTarget->BindTexture(m_NumRenderTarget++, t);

		AddOutputEelement(ShaderType::Pixel, JGShader::ToShaderDataType(format), name, "SV_TARGET" + to_string(numRT));
	}
	void GraphicsShaderModule::AddRenderTargetCubeTexture(const std::string& name, DXGI_FORMAT format)
	{
		Texture t(name);
		t.CreateResource(CD3DX12_RESOURCE_DESC::Tex2D(format, m_Width, m_Height,
			6, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET));
		uint32_t numRT = m_NumRenderTarget;
		m_RenderTarget->BindTexture(m_NumRenderTarget++, t);

		AddOutputEelement(ShaderType::Pixel, JGShader::ToShaderDataType(format), name, "SV_TARGET" + to_string(numRT));
	}
	void GraphicsShaderModule::AddDepthStencilTexture(const std::string& name, DXGI_FORMAT format)
	{
		Texture t(name);
		t.CreateResource(CD3DX12_RESOURCE_DESC::Tex2D(format, m_Width, m_Height, 1,
			0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL));
		m_RenderTarget->BindDepthTexture(t);
	}
	void GraphicsShaderModule::AddDepthStencilCubeTexture(const std::string& name, DXGI_FORMAT format)
	{
		Texture t(name);
		t.CreateResource(CD3DX12_RESOURCE_DESC::Tex2D(format, m_Width, m_Height, 6,
			0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL));
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
	void GraphicsShaderModule::SetRenderTargetTextureName(
		const std::string& origin_name, const std::string& name)
	{

	}
	void GraphicsShaderModule::SetRenderTargetTextureFormat(
		const std::string& name, DXGI_FORMAT format)
	{

	}
	void GraphicsShaderModule::SetRenderTargetTextureName(uint32_t slot, const std::string& name)
	{
	}
	void GraphicsShaderModule::SetRenderTargetTextureFormat(uint32_t slot, DXGI_FORMAT format)
	{
	}
	void GraphicsShaderModule::SetDepthStencilTextureName(const std::string& name)
	{

	}
	void GraphicsShaderModule::SetDepthStencilTextureFormat(DXGI_FORMAT format)
	{

	}

	void GraphicsShaderModule::SetInputElementSementic(
		const std::string& name, const std::string& sementic)
	{

	}
	void GraphicsShaderModule::SetInputElementName(
		const std::string& origin_name, const std::string& name)
	{

	}
	void GraphicsShaderModule::SetInputElementDataType(
		const std::string& name, JGShader::EShaderData type)
	{

	}
	void GraphicsShaderModule::SetOutputElementSementic(
		const std::string& name, const std::string& sementic)
	{

	}
	void GraphicsShaderModule::SetOutputElementName(
		const std::string& origin_name, const std::string& name)
	{

	}
	void GraphicsShaderModule::SetOutputElementDataType(
		const std::string& name, JGShader::EShaderData type)
	{

	}


	std::shared_ptr<GraphicsPipelineState> GraphicsShaderModule::MakePipelineState() const
	{
		return nullptr;
	}
	bool GraphicsShaderModule::Compile()
	{
		bool result = true;
		// RootSignature
		/*
		1. Register Space, Number, 관리
		2. 
		*/
		
		m_RootSignature = make_shared<RootSignature>();
		



		// Shader 컴파일
		for (auto& pair : m_ModuleDatasByShaderType)
		{
			auto& shader_type = pair.first;
			auto& module_data = pair.second;

			// 입력 출력 요소가 없거나 메인코드가 없다면
			// 컴파일 하지 않는다.
			if (module_data.MainCode.length() == 0 ||
				m_Input[shader_type].empty() ||
				m_Output[shader_type].empty())
				continue;
			auto LibManager = GetShaderLibManager();

			string code;
			code += "// ShaderLibraryCode Part      \n\n\n\n";
			code += LibManager->GetCode();

			code += "\n\n\n\n";
			code += "// ShaderModule Part";
			code += "\n\n\n\n";

			code += GetCode(shader_type);

		
			module_data._Shader = Shader::MakeShader(shader_type);
			module_data._Shader->Set(code);
			auto device = GetRenderDevice();
			auto is_success = device->CompileShader(*module_data._Shader, module_data.Compiler);

			if (is_success)
			{
				module_data.SafeCode = code;
			}
			else
				result = false;
		}
		

		return result;
	}
	string GraphicsShaderModule::GetCode(ShaderType type)
	{
		string code;
		auto& moduleData = m_ModuleDatasByShaderType[type];

		// 텍스쳐
		// 샘플러
		uint32_t idx = 0;
		for (auto& ss : moduleData.SSPool)
		{
			code += "SamplerState " + ss.first + " : register(s" + to_string(idx++) + ");\n";
		}
		for (auto& sbd : moduleData.SBDPool)
		{
			code += sbd.second->GetCode(0,0);
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


		code += moduleData.MainCode;

		code += "};\n";
		return code;
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
			DataIO::write(fout, t->GetDesc().DepthOrArraySize);
			DataIO::write(fout, t->GetDesc().Format);
		}
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
			DXGI_FORMAT format;
			DataIO::read(fin, name);
			DataIO::read(fin, array_size);
			DataIO::read(fin, format);

			if (array_size == 6)
			{
				AddRenderTargetTexture(name, format);
			}
			else
			{
				AddRenderTargetCubeTexture(name, format);
			}
		
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
				m_Output[shader_type].push_back(e);
			}
		}
	}


	GBufferModule::GBufferModule(const std::string& name) : GraphicsShaderModule(name) 
	{
		
	}
	bool GBufferModule::Load(const std::string& path)
	{
		if (GraphicsShaderModule::Load(path))
			return true;
		// Vertex //
		// 		
		AddStructuredBuffer("GameObjectArray", "GameObject", ShaderType::Vertex);
		auto cbuffer = AddConstantBuffer("CameraCB", ShaderType::Vertex);
		cbuffer->Add("Camera", "camera");
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

float4 posW = mul(float4(input.Position, 1.0f), obj.World);

output.PosH = mul(posW, camera.ViewProj);
output.PosW = posW.xyz;
output.NormalW = normalize(mul(input.Normal, (float3x3)obj.World));
output.TangentW = normalize(mul(input.Tangent, (float3x3)obj.World));
output.Depth = output.PosH.z / output.PosH.w;
output.TexC = input.TexC;
output.InstanceID = instanceID;


return output;

)");






		// Pixel //
		//AddSamplerState("")
		AddTexture2D("MaterialTextures", ShaderType::Pixel);
		cbuffer = AddConstantBuffer("CameraCB", ShaderType::Pixel);
		cbuffer->Add("Camera", "camera");

		AddInputEelement(ShaderType::Pixel, JGShader::_float4, "PosH", "SV_POSITION");
		AddInputEelement(ShaderType::Pixel, JGShader::_float3, "PosW", "POSITION");
		AddInputEelement(ShaderType::Pixel, JGShader::_float3, "NormalW", "NORMAL");
		AddInputEelement(ShaderType::Pixel, JGShader::_float3, "TangentW", "TANGENT");
		AddInputEelement(ShaderType::Pixel, JGShader::_float2, "TexC", "TEXCOORD");
		AddInputEelement(ShaderType::Pixel, JGShader::_float,  "Depth", "DEPTH");
		AddInputEelement(ShaderType::Pixel, JGShader::_uint,   "InstanceID", "INSTANCE");

		AddRenderTargetCubeTexture("Default", DXGI_FORMAT_R8G8B8A8_UNORM);

		SetMainCode(ShaderType::Pixel,
			R"(

Output output;

output.Default = float4(1.0f,1.0f,1.0f,1.0f);


return output;

)");

		return true;
	}
}




