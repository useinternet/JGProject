#include "pch.h"
#include "DX12_GraphicsRenderer.h"
#include "DX12_Material.h"
#include "DX12_SceneLight.h"
#include "ShaderDefined.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Texture.h"

using namespace std;

#define PASS_ENTRY "../Source/Shader/Graphics/Entry.hlsl"
#define PASS_LIGHT "../Source/Shader/Graphics/LightingPass.hlsl"

namespace DX12
{
	DX12_GraphicsRenderer::DX12_GraphicsRenderer() :
		GE::GraphicsRenderer()
	{
		Init();
	}

	DX12_GraphicsRenderer::~DX12_GraphicsRenderer()
	{
		GraphicPassMap::ResetRootSig();
	}

	bool DX12_GraphicsRenderer::DrawCall(GE::Scene* scene)
	{
		if (!m_IsSuccessed) return false;

		GraphicPassMap* passMap = nullptr;
		{
			bool is_not_find = false;
			SceneInfo* info = nullptr;
			{
				shared_lock lock(m_PassDataSceneMutex);
				auto iter = m_PassDataByScene.find(scene);
				is_not_find = iter == m_PassDataByScene.end();
				if (!is_not_find) info = &m_PassDataByScene[scene];
			}
			if (is_not_find)
			{
				lock_guard<shared_mutex> lock(m_PassDataSceneMutex);
				m_PassDataByScene[scene].passDatas.resize(3);
				uint64_t bufferCnt = DXDevice::GetBufferCount();
				for (int i = 0; i < bufferCnt; ++i)
				{
					m_PassDataByScene[scene].passDatas[i] = CreatePassData();
				}
				info = &m_PassDataByScene[scene];
			}
			info->dead_frame = 0;
			passMap = info->passDatas[DXDevice::GetBufferIndex()];
		}
		if (passMap == nullptr) return false;


		GPRunData data;
		data.OwnerRenderer = this;
		data.Scene = scene;
		passMap->Run(data);



		{
			auto t = (DX12::Texture*)scene->GetSceneTexture()->GetUserTextureData();
			DXCommand::CopyResource(DXDevice::RequestGraphicsCommandKey(), 
				*t, passMap->GetPass(data.GetFinalHashCode())->RT[0]);
		}

		// 씬 Flush
		scene->FlushObjects();
		
		return true;
	}
	bool DX12_GraphicsRenderer::CompileMaterial(GE::Material* mat)
	{

	
		if (!mat->IsDirty())
		{
			return true;
		}


		// SDStatic pso 제작
		{
			switch (mat->GetMaterialProperty().rendererMode)
			{
			case GE::RendererMode::GUI:
			case GE::RendererMode::Paper:
				if (!MakePSO(mat, GE::ObjectType::SDPaper))
					return false;
				break;
			case GE::RendererMode::_3D:
				if (!MakePSO(mat, GE::ObjectType::SDStatic))
					return false;
				if (!MakePSO(mat, GE::ObjectType::SDSkinned))
					return false;
				break;
			}


		}

		
		mat->ClearDirty();

		return true;

	}

	void DX12_GraphicsRenderer::BeginFrame()
	{
		for (auto& iter = m_PassDataByScene.begin(); iter != m_PassDataByScene.end();)
		{
			iter->second.dead_frame += 1;
			if (iter->second.dead_frame >= m_DeadLine)
			{
				uint64_t bufferCnt = DXDevice::GetBufferCount();
				for (int i = 0; i < bufferCnt; ++i)
				{
					m_WaitingPassData.push(iter->second.passDatas[i]);
				}
				iter = m_PassDataByScene.erase(iter);
			}
			else ++iter;
		}
	}

	void DX12_GraphicsRenderer::EndFrame()
	{
		
	}

	bool DX12_GraphicsRenderer::GetPSO(GE::SceneObject* obj, GraphicsPipelineState& pso)
	{
		if (obj->GetMaterial() == nullptr) return false;

		GE::Material* mat = obj->GetMaterial()->GetOwnerMaterial();
		size_t hash = (size_t)mat;
		dx_12_hash_combine(hash, obj->GetObjectType());


		if (!CompileMaterial(mat))
			return false;

		shared_lock lock(m_MaterialPSOsMutex);
		pso = m_MaterialPSOs[hash];
		return true;
	}


	void DX12_GraphicsRenderer::Init()
	{
		//
		// 루트서명 초기화
		// InitRootSignature();
		GBuffer::ShaderPath  = "../Source/Shader/Graphics/Entry.hlsl";
		Lighting::ShaderPath = "../Source/Shader/Graphics/LightingPass.hlsl";
		ShadowMap::ShaderPath = "../Source/Shader/Graphics/ShadowMap.hlsl";


		RootSignature rootSig;
	    rootSig.InitAsCBV(0, 0); // Camera
		rootSig.InitAsCBV(1, 0); // Material
		rootSig.InitAsCBV(2, 0); // AnimData
		rootSig.InitAsCBV(3, 0); // DirectionalLight
		rootSig.InitAsCBV(4, 0); // PassData
		// 
		rootSig.InitAsSRV(0, 0); // Object
		rootSig.InitAsSRV(0, 1); // PointLight
		rootSig.InitAsSRV(0, 2); // SpotLight


		rootSig.InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 0, 3);    // InputTexture
		rootSig.InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1000, 0, 4); // BindTexture
		rootSig.InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1000, 0, 5); // SLShadowMap
		rootSig.InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1000, 0, 6); // PLShadowMap
		rootSig.InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1000, 8, 3); // DLShadowMap


		rootSig.AddStaticSamplerState(CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_ANISOTROPIC));
		rootSig.AddStaticSamplerState(CD3DX12_STATIC_SAMPLER_DESC(1, D3D12_FILTER_MIN_MAG_MIP_POINT));
		rootSig.AddStaticSamplerState(CD3DX12_STATIC_SAMPLER_DESC(2, D3D12_FILTER_MIN_MAG_MIP_LINEAR));
		rootSig.AddStaticSamplerState(CD3DX12_STATIC_SAMPLER_DESC(
			3, 
			D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_BORDER, 
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,
			0.0f, 16, 
			D3D12_COMPARISON_FUNC_LESS_EQUAL));



		if (!rootSig.Finalize())
		{
			m_IsSuccessed = false;
			GELOG_FATAL("루트 서명 생성에 실패했습니다.");
		}
		GraphicPassMap::SetRootSig(rootSig);
		m_OriginPassMap.AddPass<Entry>();
		m_OriginPassMap.AddPass<GBuffer>();
		m_OriginPassMap.AddPass<Lighting>();
		m_OriginPassMap.AddPass<ShadowMap>();

		m_OriginPassMap.SetEntryPass<Entry>();
	}

	bool DX12_GraphicsRenderer::MakePSO(GE::Material* mat, GE::ObjectType objectType)
	{
	

		GraphicsPipelineState pso;
		InputLayout inputLayout;
		string objectMacro;
		string rendererMacro;
		
		size_t hash = (size_t)mat;
		dx_12_hash_combine(hash, objectType);

		GE::MaterialProperty property = mat->GetMaterialProperty();

		// RootSignature 과 RenderTarget 양식 바인딩
		{
			pso.BindRootSignature(GraphicPassMap::GetRootSig());
			pso.BindRenderTarget({
				DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM }, 
				DXGI_FORMAT_D24_UNORM_S8_UINT);
		}
		// ObjectType별 InputLayout 정의
		{
			switch (objectType)
			{
			case GE::ObjectType::SDStatic:
				inputLayout.Add("POSITION", ShaderDataType::_float3_, 0);
				inputLayout.Add("NORMAL", ShaderDataType::_float3_, 0);
				inputLayout.Add("TANGENT", ShaderDataType::_float3_, 0);
				inputLayout.Add("TEXCOORD", ShaderDataType::_float2_, 0);
				objectMacro = SHADER_MACRO_DEFINE_SDSTATIC_OBJECT;
				break;

			case GE::ObjectType::SDSkinned:
				inputLayout.Add("POSITION", ShaderDataType::_float3_, 0);
				inputLayout.Add("NORMAL", ShaderDataType::_float3_, 0);
				inputLayout.Add("TANGENT", ShaderDataType::_float3_, 0);
				inputLayout.Add("TEXCOORD", ShaderDataType::_float2_, 0);
				inputLayout.Add("BONEWEIGHT", ShaderDataType::_float4_, 0);
				inputLayout.Add("BONEID", ShaderDataType::_int4_, 0);
				objectMacro = SHADER_MACRO_DEFINE_SDSKINNED_OBJECT;
				break;
			case GE::ObjectType::SDPaper:
				inputLayout.Add("POSITION", ShaderDataType::_float3_, 0);
				inputLayout.Add("TEXCOORD", ShaderDataType::_float2_, 0);
				objectMacro = SHADER_MACRO_DEFINE_SDPAPER_OBJECT;
				break;
			default:
				GELOG_ERROR("지원하지 않는 오브젝트 타입");
				return false;
			}
			pso.BindInputLayout(inputLayout);
		}

		switch (property.rendererMode)
		{
		case GE::RendererMode::_3D:
			rendererMacro = SHADER_MACRO_DEFINE_RENDERER_MODE_3D; break;
		case GE::RendererMode::Paper:
			rendererMacro = SHADER_MACRO_DEFINE_RENDERER_MODE_PAPER; break;
		case GE::RendererMode::GUI:
			rendererMacro = SHADER_MACRO_DEFINE_RENDERER_MODE_GUI; 			
			break;
		default:
			GELOG_ERROR("지원하지 않는 렌더링 모드");
			return false;
		}

		switch (property.blendMode)
		{
		case GE::BlendMode::Transparent:
			{
				D3D12_BLEND_DESC desc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
				desc.RenderTarget[0].BlendEnable  = true;
				desc.RenderTarget[0].BlendOp      = D3D12_BLEND_OP_ADD;
				desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
				desc.RenderTarget[0].DestBlend    = D3D12_BLEND_INV_SRC_ALPHA;
				desc.RenderTarget[0].DestBlendAlpha        = D3D12_BLEND_ONE; // D3D12_BLEND_ZERO;
				desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
				desc.RenderTarget[0].SrcBlend              = D3D12_BLEND_SRC_ALPHA;
				desc.RenderTarget[0].SrcBlendAlpha         = D3D12_BLEND_INV_DEST_ALPHA; // D3D12_BLEND_ONE;
				pso.SetBlendState(desc);				
			}
			{
				D3D12_DEPTH_STENCIL_DESC desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				desc.DepthEnable = false;
				pso.SetDepthStencilState(desc);
			}
			break;
		}


		VertexShader vShader;
		PixelShader  pShader;
		// Material 매크로 및 셰이더 컴파일 및 바인딩
		{
			string matCode = mat->GetCode();
			string matCBCode = mat->GetMaterialCBCode();



			auto dataNameList = mat->GetMaterialDataNameList();
			auto textureNameList = mat->GetMaterialTextureNameList();


			for (auto& name : dataNameList)
			{
				matCBCode = replaceAll(matCBCode, name + SHADER_NAME_SUFFIX_MATERIAL_DATA, "MATERIAL_DATA_" + name);
				matCode   = replaceAll(matCode, name + SHADER_NAME_SUFFIX_MATERIAL_DATA, "MATERIAL_DATA_" + name);
			}

			uint32_t tIndex = 0;
			for (auto& name : textureNameList)
			{
				matCode = replaceAll(matCode, name + SHADER_NAME_SUFFIX_TEXTURE, "gBindTexture[" + to_string(tIndex++) + "]");
			}
			matCBCode = replaceAll(matCBCode, "\n", "");
			matCode = replaceAll(matCode, "\n", "");
			matCode = replaceAll(matCode, "\t", "");





			
			vShader = DXDevice::CreateVertexShaderFromFile(PASS_ENTRY,
				{ pair<string,string>(SHADER_MACRO_DEFINE_MATERIAL_CODE, matCode),
				  pair<string,string>(SHADER_MACRO_DEFINE_MATERIAL_DATA, matCBCode),
				  pair<string,string>(objectMacro, ""), 
				  pair<string,string>(rendererMacro, "") });

			pShader = DXDevice::CreatePixelShaderFromFile(PASS_ENTRY,
				{ pair<string,string>(SHADER_MACRO_DEFINE_MATERIAL_CODE, matCode),
				  pair<string,string>(SHADER_MACRO_DEFINE_MATERIAL_DATA, matCBCode),
			      pair<string,string>(objectMacro, ""),
			      pair<string,string>(rendererMacro, "") });

			if (!vShader.IsValid() || !pShader.IsValid())
			{
				GELOG_ERROR("셰이더 컴파일 에러 : (%s) , (%s)", 
					s2ws(vShader.GetErrorCode()).c_str(), s2ws(pShader.GetErrorCode()).c_str());
				return false;
			}

			pso.BindShader(ShaderType_Vertex, vShader);
			pso.BindShader(ShaderType_Pixel, pShader);
		}


		// 상태 저장
		{



			pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		}

		if (!pso.Finalize())
		{
			GELOG_ERROR("PipelineState 생성 실패");
			return false;
		}
		{
			lock_guard<shared_mutex> lock(m_MaterialPSOsMutex);
			m_MaterialPSOs[hash] = pso;
		}


		return true;
	}

	GraphicPassMap* DX12_GraphicsRenderer::CreatePassData()
	{
		uint64_t bufferIndex = DXDevice::GetBufferIndex();
		{
			if (!m_WaitingPassData.empty())
			{
				auto passData = m_WaitingPassData.front(); m_WaitingPassData.pop();
				return passData;
			}

		}
		{
			auto passData = m_OriginPassMap.Copy();
			auto passDataPtr = passData.get();

			m_PassDataPool.push_back(move(passData));
			return passDataPtr;
		}
		

		return nullptr;
	}

}