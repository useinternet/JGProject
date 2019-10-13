#include "pch.h"
#include <d3dcompiler.h>
#include "ShaderModule.h"
#include "Object/DxObject/RootSignature.h"
#include "ShaderRegisterMap.h"
#include "Object/DxObject/CommandList.h"
#include "Object/DxObject/RenderDevice.h"
#include "Object/ReObject/RenderTarget.h"
#include "Object/DxObject/PipelineState.h"
using namespace std;
#define OUT_STREAM "OutStream" 
namespace RE
{
	ShaderModule::ShaderModule() : ReObject("ShaderModule") 
	{
		m_RegisterMap = make_shared<ShaderRegisterMap>();

		m_ShaderDatas[ShaderType::Vertex].Compiler = ShaderCompiler(ShaderType::Vertex);
		m_ShaderDatas[ShaderType::Hull].Compiler = ShaderCompiler(ShaderType::Hull);
		m_ShaderDatas[ShaderType::Domain].Compiler = ShaderCompiler(ShaderType::Domain);
		m_ShaderDatas[ShaderType::Geometry].Compiler = ShaderCompiler(ShaderType::Geometry);
		m_ShaderDatas[ShaderType::Pixel].Compiler = ShaderCompiler(ShaderType::Pixel);
		m_ShaderDatas[ShaderType::Compute].Compiler = ShaderCompiler(ShaderType::Compute);

		for (auto& data : m_ShaderDatas)
		{
			data.second.Compiler.Flags1 = D3DCOMPILE_DEBUG;
		}
	}
	SCVar* ShaderModule::DefineVar(ShaderEnum::EShaderDataType type, const std::string& name, ShaderEnum::EShaderVisible visible)
	{
		shared_ptr<SCVar> var = make_shared<SCVar>();
		var->SetOwner(this);
		var->Set(type, name);
	

		m_ShaderDefineCodes.emplace(var->GetName(), var);
		m_DefinedShaderVars.push_back(var.get());
		return var.get();
	}

	SCStruct* ShaderModule::DefineStruct(const std::string& name, ShaderEnum::EShaderVisible visible)
	{
		shared_ptr<SCStruct> _struct = make_shared<SCStruct>();
		_struct->SetOwner(this);
		_struct->Set(name, visible);
	
		m_ShaderDefineCodes.emplace(_struct->GetName(), _struct);
		m_DefinedShaderStruct.push_back(_struct.get());
		return _struct.get();
	}

	SCInputStruct* ShaderModule::DefineInputStruct(const std::string& name, ShaderEnum::EShaderVisible visible)
	{
		shared_ptr<SCInputStruct> inputStruct = make_shared<SCInputStruct>();
		inputStruct->SetOwner(this);
		inputStruct->Set(name, visible);
		
		m_ShaderDefineCodes.emplace(inputStruct->GetName(), inputStruct);
		m_DefinedShaderInputStruct.push_back(inputStruct.get());
		return inputStruct.get();
	}

	SDConstantBuffer* ShaderModule::DefineConstantBuffer(
		const std::string& name, 
		ShaderEnum::EShaderVisible visible)
	{
		shared_ptr<SDConstantBuffer> cbuffer = make_shared<SDConstantBuffer>(this);
		cbuffer->Set(name, visible);

		m_ShaderBindDatas.emplace(cbuffer->GetName(), cbuffer);
		m_BindedConstantBuffers.emplace(cbuffer->GetName(), cbuffer.get());
		m_RegisterMap->bRegister(cbuffer.get());
		return cbuffer.get();
	}

	SDStructuredBuffer* ShaderModule::DefineStructuredBuffer(
		const std::string& name, 
		const std::string& defined_struct_name, 
		ShaderEnum::EShaderVisible visible)
	{
		auto findcode = FindDefinedStruct(defined_struct_name);
		if (findcode == nullptr)
			return nullptr;
		shared_ptr<SDStructuredBuffer> sbuffer = make_shared<SDStructuredBuffer>(this);
		sbuffer->Set(name, *findcode,  visible);

		m_ShaderBindDatas.emplace(sbuffer->GetName(), sbuffer);
		m_BindedStructuredBuffers.emplace(sbuffer->GetName(), sbuffer.get());
		m_RegisterMap->tStructuredRegister(sbuffer.get());
		return sbuffer.get();
	}

	SDResource* ShaderModule::DefineSDResource(
		ShaderEnum::EShaderResourceType type,
		const std::string& name, 
		uint32_t init_array_size,
		ShaderEnum::EShaderVisible visible)
	{
		shared_ptr<SDResource> resource = make_shared<SDResource>(this);
		resource->Set(type, name, init_array_size, visible);

		m_ShaderBindDatas.emplace(resource->GetName(), resource);
		m_BindedResources.emplace(resource->GetName(), resource.get());
		switch (type)
		{
		case ShaderEnum::Texture2D:
		case ShaderEnum::RWTexture2D:
			m_RegisterMap->t2DRegister(resource.get());
			break;
		case ShaderEnum::TextureCube:
		case ShaderEnum::RWTextureCube:
			m_RegisterMap->tCubeRegister(resource.get());
			break;
		}
		return resource.get();
	}

	SDSamplerState* ShaderModule::DefineSamplerState(
		const std::string& name,
		const D3D12_STATIC_SAMPLER_DESC& desc,
		ShaderEnum::EShaderVisible visible)
	{
		shared_ptr<SDSamplerState> sampler = make_shared<SDSamplerState>(this);
		sampler->Set(desc, name, visible);

		m_ShaderBindDatas.emplace(sampler->GetName(), sampler);
		m_BindedSamplerStates.emplace(sampler->GetName(), sampler.get());
		m_RegisterMap->sRegister(sampler.get());
		return sampler.get();
	}

	SCVar* ShaderModule::FindDefinedVar(const std::string& name)
	{
		for (auto& var : m_DefinedShaderVars)
		{
			if (var->GetName() == name)
				return var;
		}
		return nullptr;
	}

	SCStruct* ShaderModule::FindDefinedStruct(const std::string& name)
	{
		for (auto& var : m_DefinedShaderStruct)
		{
			if (var->GetName() == name)
				return var;
		}
		return nullptr;
	}

	SCInputStruct* ShaderModule::FindDefinedInputStruct(const std::string& name)
	{
		for (auto& var : m_DefinedShaderInputStruct)
		{
			if (var->GetName() == name)
				return var;
		}
		return nullptr;
	}

	SDConstantBuffer* ShaderModule::FindBindedContantBuffer(const std::string& name)
	{
		if (m_BindedConstantBuffers.find(name) == m_BindedConstantBuffers.end())
			return nullptr;
		return m_BindedConstantBuffers[name];
	}

	SDStructuredBuffer* ShaderModule::FindBindedStructuredBuffer(const std::string& name)
	{
		if (m_BindedStructuredBuffers.find(name) == m_BindedStructuredBuffers.end())
			return nullptr;
		return m_BindedStructuredBuffers[name];
	}

	SDSamplerState* ShaderModule::FindBindedSamplerState(const std::string& name)
	{
		if (m_BindedSamplerStates.find(name) == m_BindedSamplerStates.end())
			return nullptr;
		return m_BindedSamplerStates[name];
	}

	SDResource* ShaderModule::FindBindedSDResource(const std::string& name)
	{
		if (m_BindedResources.find(name) == m_BindedResources.end())
			return nullptr;
		return m_BindedResources[name];
	}
	void ShaderModule::SetInputOutputStream(
		ShaderType type,
		const std::string& in_inputstruct_name,
		const std::string& out_inputstruct_name)
	{
		m_ShaderDatas[type].Stream = InputOutputStream(in_inputstruct_name, out_inputstruct_name);
	}
	void ShaderModule::CreateRootSignature()
	{
		m_ConstantBufferRootParamMap.clear();


		size_t paramSize = m_BindedConstantBuffers.size();
	

		paramSize += m_BindedStructuredBuffers.size();
		paramSize += m_BindedResources.size();
		m_RootSignature = make_shared<RootSignature>(paramSize);
		// constantbuffer
		int idx = 0;
		for (auto& cbuffer : m_BindedConstantBuffers)
		{
			m_RootSignature->InitParam(idx).InitAsConstantBufferView(
				cbuffer.second->GetRegisterNumber(),
				cbuffer.second->GetRegisterSpace(),
				ShaderEnum::ShaderVisibleToDx12(cbuffer.second->GetVisible()));
			m_ConstantBufferRootParamMap[cbuffer.first] = idx++;
		}
		// structuredbuffer
		for (auto& sbuffer : m_BindedStructuredBuffers)
		{
			m_RootSignature->InitParam(idx).InitAsSRV(
				sbuffer.second->GetRegisterNumber(),
				sbuffer.second->GetRegisterSpace(),
				ShaderEnum::ShaderVisibleToDx12(sbuffer.second->GetVisible()));
			m_StructuredBufferRootParamMap[sbuffer.first] = idx++;
		}
		// resource
		for (auto& resource : m_BindedResources)
		{
			D3D12_DESCRIPTOR_RANGE_TYPE rangeType;
			switch (resource.second->GetResourceType())
			{
			case ShaderEnum::Texture2D:
			case ShaderEnum::TextureCube:
				rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				break;
			case ShaderEnum::RWTexture2D:
			case ShaderEnum::RWTextureCube:
				rangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				break;
			}
			m_RootSignature->InitParam(idx).PushAsDescriptorRange(
				rangeType, resource.second->GetBindedResourceSize(),
				resource.second->GetRegisterNumber(), resource.second->GetRegisterSpace());
			m_RootSignature->InitParam(idx).InitAsDescriptorTable(
				ShaderEnum::ShaderVisibleToDx12(resource.second->GetVisible()));
			m_ResourceRootParamMap[resource.first] = idx++;
		}



		for (auto& sampler : m_BindedSamplerStates)
		{
			m_RootSignature->AddStaticSampler(sampler.second->GetDesc());
		}

		if (!m_RootSignature->Finalize())
		{
			RE_LOG_ERROR("Failed Create RootSignature in {0}", GetName());
			return;
		}
	}
	bool ShaderModule::Compile(ErrorCodeMap& out_error_code)
	{
		std::map<ShaderType, std::shared_ptr<Shader>> shader_datas;
		bool finalResult = true;
		for (auto& data : m_ShaderDatas)
		{
			if (data.second.MainCode.size() == 0)
			{
				continue;
			}
			switch (data.first)
			{
			case ShaderType::Vertex:
				shader_datas[data.first] = make_shared<VertexShader>();
				break;
			case ShaderType::Hull:
				shader_datas[data.first] = make_shared<HullShader>();
				break;
			case ShaderType::Domain:
				shader_datas[data.first] = make_shared<DomainShader>();
				break;
			case ShaderType::Geometry:
				shader_datas[data.first] = make_shared<GeometryShader>();
				break;
			case ShaderType::Pixel:
				shader_datas[data.first] = make_shared<PixelShader>();
				break;
			case ShaderType::Compute:
				shader_datas[data.first] = make_shared<ComputeShader>();
				break;
			}
			shader_datas[data.first]->Set(data.second.FinalCode);
			bool result = GetRenderDevice()->CompileShader(*shader_datas[data.first], data.second.Compiler);
			if (!result)
			{
				out_error_code[data.first].push_back(shader_datas[data.first]->GetErrorCode());
				finalResult = false;
			}
		}
		if (finalResult)
		{
			for (auto& data : m_ShaderDatas)
			{
				data.second.Data = move(shader_datas[data.first]);
			}
		}
		return finalResult;
	}
	std::string ShaderModule::GetCode(ShaderType type)
	{
		return m_ShaderDatas[type].FinalCode;
	}

	RootSignature* ShaderModule::GetRootSignature() const
	{
		return m_RootSignature.get();
	}

	ShaderRegisterMap* ShaderModule::GetRegisterMap() const
	{
		return m_RegisterMap.get();
	}
	const Shader& ShaderModule::GetShader(ShaderType type) const
	{
		return *m_ShaderDatas.at(type).Data;
	}
	bool ShaderModule::DuplicateCheck(const std::string& name)
	{
		if (m_ShaderBindDatas.find(name) != m_ShaderBindDatas.end())
		{
			RE_LOG_ERROR("Name: {0} Duplicated in {1}", name, GetName());
			return true;
		}
		if (m_ShaderDefineCodes.find(name) != m_ShaderDefineCodes.end())
		{
			RE_LOG_ERROR("Name: {0} Duplicated in {1}", name, GetName());
			return true;
		}
		return false;
	}
	void GraphicsShaderModule::PushBindedDataInCommandList(CommandList* cmdList)
	{
		for (auto& rootparam : m_ConstantBufferRootParamMap)
		{
			auto cbuffer = m_BindedConstantBuffers[rootparam.first];
			cmdList->BindGraphicsDynamicConstantBuffer(rootparam.second, cbuffer->GetData());
		}
		for (auto& rootparam : m_StructuredBufferRootParamMap)
		{
			auto sbuffer = m_BindedStructuredBuffers[rootparam.first];
			cmdList->BindGraphicsDynamicStructuredBuffer(rootparam.second, sbuffer->GetData());
		}
		for (auto& rootparam : m_ResourceRootParamMap)
		{
			auto resource = m_BindedResources[rootparam.first];
			auto type = resource->GetResourceType();

			switch (type)
			{
			case ShaderEnum::Texture2D:
			case ShaderEnum::TextureCube:
				cmdList->BindSRV(rootparam.second, resource->GetBindedResources());
				break;
			case ShaderEnum::RWTexture2D:
			case ShaderEnum::RWTextureCube:
				cmdList->BindUAV(rootparam.second, resource->GetBindedResources());
				break;
			}
		}
	}

	void GraphicsShaderModule::PushCode(ShaderType type, const std::string& code)
	{
		if (type == ShaderType::Compute)
		{
			RE_LOG_ERROR("{0} is not support compute shader", GetName());
			return;
		}
		m_ShaderDatas[type].MainCode = code;
	}
	
	void GraphicsShaderModule::CreateCode()
	{
		for (auto& var : m_DefinedShaderVars)
		{
			InsertCodeByVisible(var->GetVisible(), var->GetShaderCode());
		}
		for (auto& var : m_DefinedShaderStruct)
		{
			InsertCodeByVisible(var->GetVisible(), var->GetShaderCode());
		}
		for (auto& var : m_DefinedShaderInputStruct)
		{
			InsertCodeByVisible(var->GetVisible(), var->GetShaderCode());
		}
		for (auto& resource : m_BindedResources)
		{
			InsertCodeByVisible(resource.second->GetVisible(), resource.second->GetShaderCode());
		}
		for (auto& sbuffer : m_BindedStructuredBuffers)
		{
			InsertCodeByVisible(sbuffer.second->GetVisible(), sbuffer.second->GetShaderCode());
		}
		for (auto& sampler : m_BindedSamplerStates)
		{
			InsertCodeByVisible(sampler.second->GetVisible(), sampler.second->GetShaderCode());
		}
		for (auto& cbuffer : m_BindedConstantBuffers)
		{
			InsertCodeByVisible(cbuffer.second->GetVisible(), cbuffer.second->GetShaderCode());
		}

		for (auto& code : m_ShaderDatas)
		{
			std::string main_code = m_ShaderDatas[code.first].Stream.second + " ";
			main_code += m_ShaderDatas[code.first].Compiler.EntryPoint +
				"(" + m_ShaderDatas[code.first].Stream.first + " " + m_ShaderDatas[code.first].Compiler.inStream + ") \n";
			main_code += "{\n   ";
			main_code += m_ShaderDatas[code.first].Stream.second + " " + m_ShaderDatas[code.first].Compiler.outStream +  ";\n";


			main_code += code.second.MainCode;

	
			main_code += "\n   return " + m_ShaderDatas[code.first].Compiler.outStream + ";\n";
			main_code += "};\n";

			code.second.FinalCode += main_code;
		}
	}
	void GraphicsShaderModule::Execute(CommandList* cmdList, const PipelineState& pso)
	{
		cmdList->SetGraphicsRootSignature(*m_RootSignature);
		cmdList->SetViewport(m_Viewport);
		cmdList->SetScissorRect(m_ScissorRect);
		cmdList->SetPipelineState(pso);
		cmdList->SetRenderTarget(*m_RenderTarget);
	

		for (auto& cbuffer : m_BindedConstantBuffers)
		{
			if (m_ConstantBufferRootParamMap.find(cbuffer.first) == m_ConstantBufferRootParamMap.end())
				continue;

			cmdList->BindGraphicsDynamicConstantBuffer(
				m_ConstantBufferRootParamMap[cbuffer.first], cbuffer.second->GetData());
		}

		for (auto& sbuffer : m_BindedStructuredBuffers)
		{
			if (m_StructuredBufferRootParamMap.find(sbuffer.first) == m_StructuredBufferRootParamMap.end())
				continue;

			cmdList->BindGraphicsDynamicStructuredBuffer(
				m_StructuredBufferRootParamMap[sbuffer.first],
				sbuffer.second->GetElementSize(),
				sbuffer.second->GetData());
		}
		for (auto& sd_resource : m_BindedResources)
		{
			if (m_ResourceRootParamMap.find(sd_resource.first) == m_ResourceRootParamMap.end())
				continue;

			switch (sd_resource.second->GetResourceType())
			{
			case ShaderEnum::Texture2D:
			case ShaderEnum::TextureCube:
				cmdList->BindSRV(m_ResourceRootParamMap[sd_resource.first], sd_resource.second->GetBindedResources());
				break;
			case ShaderEnum::RWTexture2D:
			case ShaderEnum::RWTextureCube:
				cmdList->BindUAV(m_ResourceRootParamMap[sd_resource.first], sd_resource.second->GetBindedResources());
				break;
			}
		}

	}

	Texture* GraphicsShaderModule::AddRenderTargetTexture(
		const std::string& name,
		DXGI_FORMAT format,
		const Color& clearColor,
		uint16_t array_size,
		uint16_t miplevels,
		uint32_t sampleCount,
		uint32_t sampleQuality,
		uint64_t alignment)
	{
		if (m_NumRenderTarget > 8)
		{
			RE_LOG_ERROR("NumRenderTarget over max 8 in {0}", GetName());
			return nullptr;
		}
		auto out_stream = FindDefinedInputStruct(OUT_STREAM);
		if (out_stream == nullptr)
		{
			out_stream = DefineInputStruct(OUT_STREAM);
			m_RenderTarget = make_shared<RenderTarget>();
		}
		D3D12_CLEAR_VALUE value;
		value.Format = format;
		value.Color[0] = clearColor.r;
		value.Color[1] = clearColor.g;
		value.Color[2] = clearColor.b;
		value.Color[3] = clearColor.a;

		Texture texture(name);
		texture.CreateResource(
			CD3DX12_RESOURCE_DESC::Tex2D(
				format, m_Width, m_Height, array_size,
				miplevels, sampleCount, sampleQuality,
				D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, 
				D3D12_TEXTURE_LAYOUT_UNKNOWN,alignment), 
			&value);
		m_RenderTarget->BindTexture(m_NumRenderTarget, texture);
		m_RTTexturePool[name] = m_RenderTarget->GetTexture(m_NumRenderTarget);
		out_stream->AddVar(ShaderEnum::FormatToShaderDataType(format), name, "SV_TARGET" + to_string(m_NumRenderTarget));
		++m_NumRenderTarget;

		return m_RTTexturePool[name];
	}

	Texture* GraphicsShaderModule::AddDepthStencilTexture(
		const std::string& name,
		DXGI_FORMAT format,
		float clear_depth,
		uint8_t clear_stencil,
		uint16_t array_size,
		uint16_t miplevels,
		uint32_t sampleCount,
		uint32_t sampleQuality,
		uint64_t alignment,
		D3D12_CLEAR_FLAGS clear_flag)
	{
		D3D12_CLEAR_VALUE value;
		value.Format = format;
		value.DepthStencil.Depth = clear_depth;
		value.DepthStencil.Stencil = clear_stencil;

		Texture texture(name);
		texture.CreateResource(
			CD3DX12_RESOURCE_DESC::Tex2D(
				format, m_Width, m_Height, array_size,
				miplevels, sampleCount, sampleQuality,
				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL,
				D3D12_TEXTURE_LAYOUT_UNKNOWN, alignment),
			&value);

		m_RenderTarget->BindDepthTexture(texture, clear_flag);

		return m_RenderTarget->GetDepthTexture();
	}
	Texture* GraphicsShaderModule::FindRenderTargetTexture(const std::string& name)
	{
		if (m_RTTexturePool.find(name) == m_RTTexturePool.end())
		{
			return nullptr;
		}
		return m_RTTexturePool[name];
	}
	Texture* GraphicsShaderModule::FindRenderTargetTexture(uint32_t slot)
	{
		if (m_NumRenderTarget <= slot)
			return nullptr;
		return m_RenderTarget->GetTexture(slot);
	}
	RenderTarget* GraphicsShaderModule::GetRenderTarget() const
	{
		return m_RenderTarget.get();
	}
	std::shared_ptr<GraphicsPipelineState> GraphicsShaderModule::GetPipelineState(const D3D12_DEPTH_STENCIL_DESC& depth_desc, const D3D12_RASTERIZER_DESC& raster_desc, const D3D12_BLEND_DESC& blend_desc)
	{
		std::shared_ptr<GraphicsPipelineState> pso = make_shared<GraphicsPipelineState>();
		pso->SetDepthStencilState(depth_desc);
		pso->SetRasterizerState(raster_desc);
		pso->SetBlendState(blend_desc);



		// 렌더 타겟 및 깊이 텍스쳐 설정
		vector<uint32_t> renderSlots(m_NumRenderTarget);
		vector<DXGI_FORMAT> renderFormats(m_NumRenderTarget);
		for (int i = 0; i < m_NumRenderTarget; ++i)
		{
			renderSlots[i] = i;
			renderFormats[i] = m_RenderTarget->GetTexture(i)->GetDesc().Format;
		}

		pso->SetRenderTargetFormat(renderSlots, renderFormats);
		
		auto depth_texture = m_RenderTarget->GetDepthTexture();
		if (depth_texture)
		{
			pso->SetDepthStencilFormat(depth_texture->GetDesc().Format);
		}

		// 입력 레이아웃 바인딩
		auto input_struct_name = m_ShaderDatas[ShaderType::Vertex].Stream.first;
		auto input_strcut = FindDefinedInputStruct(input_struct_name);
		if (input_strcut)
		{
			pso->SetInputLayout(input_strcut->GetInputElementDescArray());
		}

		// 루트 서명 바인딩
		pso->SetRootSignature(*m_RootSignature);

		// 셰이더 데이터 바인딩
		for (auto& sd : m_ShaderDatas)
		{
			auto shader = sd.second.Data;

			if (shader)
			{
				pso->BindShader(*shader);
			}
		}

		if (pso->Finalize())
		{
			return pso;
		}
		else
		{
			RE_LOG_ERROR("Failed Create PipeLineState");
			return nullptr;
		}
	}

	void GraphicsShaderModule::InsertCodeByVisible(ShaderEnum::EShaderVisible visible, const std::string& src)
	{
		if (ShaderEnum::Visible_Vertex & visible)
		{
			m_ShaderDatas[ShaderType::Vertex].FinalCode += src;
		}
		if (ShaderEnum::Visible_Hull & visible)
		{
			m_ShaderDatas[ShaderType::Hull].FinalCode += src;
		}
		if (ShaderEnum::Visible_Domain & visible)
		{
			m_ShaderDatas[ShaderType::Domain].FinalCode += src;
		}
		if (ShaderEnum::Visible_Geometry & visible)
		{
			m_ShaderDatas[ShaderType::Geometry].FinalCode += src;
		}
		if (ShaderEnum::Visible_Pixel & visible)
		{
			m_ShaderDatas[ShaderType::Pixel].FinalCode += src;
		}
	}
}