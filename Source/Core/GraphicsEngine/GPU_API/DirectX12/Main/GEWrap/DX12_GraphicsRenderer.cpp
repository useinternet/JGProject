#include "pch.h"
#include "DX12_GraphicsRenderer.h"
#include "DX12_Material.h"
#include "DX12_RenderingPass.h"
#include "ShaderDefined.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Texture.h"

using namespace std;

#define PASS_ENTRY "../Source/Shader/Graphics/Entry.hlsl"
#define PASS_FINAL "../Source/Shader/Graphics/Final.hlsl"

namespace DX12
{
	DX12_GraphicsRenderer::DX12_GraphicsRenderer() :
		GE::GraphicsRenderer()
	{
		Init();
	}

	DX12_GraphicsRenderer::~DX12_GraphicsRenderer()
	{

	}

	bool DX12_GraphicsRenderer::DrawCall(GE::Scene* scene)
	{
		if (!m_IsSuccessed) return false;
		PassData* passData = nullptr;
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
			passData = info->passDatas[DXDevice::GetBufferIndex()];
		}
		if (passData == nullptr) return false;



		

		auto cmdKey = DXDevice::RequestGraphicsCommandKey();

		// 루트 서명 바인딩
		DXCommand::BindRootSignature(cmdKey, m_RootSig);

		// RootPass 실행
		{
			PassRunData runData;
			runData.cmdKey   = cmdKey;
			runData.scene    = scene;
			runData.passData = passData;
			passData->passMap[PASS_ENTRY]->Run(&runData);
		}
		// 텍스쳐 복사
		{
			auto t = (DX12::Texture*)scene->GetSceneTexture()->GetUserTextureData();
			auto desc1 = t->GetDesc();
			auto desc2 = passData->passMap[PASS_FINAL]->GetRTRef()[0].GetDesc();
			DXCommand::CopyResource(cmdKey, *t, passData->passMap[PASS_FINAL]->GetRTRef()[0]);
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


	void DX12_GraphicsRenderer::Init()
	{
		//
		// 루트서명 초기화
		{
			//
			m_RootSig.InitAsCBV(0, 0); // Camera
			m_RootSig.InitAsCBV(1, 0); // Material
			m_RootSig.InitAsCBV(2, 0); // AnimData
			// 
			m_RootSig.InitAsSRV(0, 0); // Object



			m_RootSig.InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 8, 0, 1);
			m_RootSig.InitAsDescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1000, 0, 2);



			m_RootSig.AddStaticSamplerState(CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_ANISOTROPIC));
			m_RootSig.AddStaticSamplerState(CD3DX12_STATIC_SAMPLER_DESC(1, D3D12_FILTER_MIN_MAG_MIP_POINT));
			m_RootSig.AddStaticSamplerState(CD3DX12_STATIC_SAMPLER_DESC(2, D3D12_FILTER_MIN_MAG_MIP_LINEAR));

			if (!m_RootSig.Finalize())
			{
				m_IsSuccessed = false;
				GELOG_FATAL("루트 서명 생성에 실패했습니다.");
			}
		}
		// PassPool 삽입
		// PassEntry
		m_PassSetUpTaskGroup.run([&]()
		{
			PassSetUpData setUp;
			setUp.name = PASS_ENTRY;
			setUp.ownerRenderer = this;
			setUp.setUpFunc = [&](RenderTarget& renderTarget, GraphicsPipelineState& pso) -> bool
			{
				renderTarget[0].CreateRenderTargetTexture(TT("Albedo"), DXGI_FORMAT_R8G8B8A8_UNORM, 1920, 1080, 1);
				renderTarget.GetDepthStencilTexture().CreateDepthStencilTexture(TT("DepthStencil"),
					DXGI_FORMAT_D24_UNORM_S8_UINT, 1920, 1080, 1);

				return true;
			};

			setUp.runFunc = [&](DX12_RenderingPass* pass,PassRunData& data) -> bool
			{
				PassData* passDataPtr = (PassData*)data.passData;
				auto bindedCam = data.scene->GetBindedCamera();
				auto cbCam = data.scene->GetBindedCamera()->GetCameraCB();
				auto objs = data.scene->GetPushedObjects();
				pass->GetRTRef().Resize(cbCam.lensWidth, cbCam.lensHeight);

				DXCommand::SetViewport(data.cmdKey, Viewport(cbCam.lensWidth, cbCam.lensHeight));
				DXCommand::SetScissorRect(data.cmdKey, ScissorRect(cbCam.lensWidth, cbCam.lensHeight));
				DXCommand::ClearRenderTarget(data.cmdKey, pass->GetRTRef());
				DXCommand::SetRenderTarget(data.cmdKey, pass->GetRTRef());

				DXCommand::BindDynamicConstantBuffer(data.cmdKey, (int)RootParam::Camera, &cbCam, sizeof(cbCam));
				for (auto& obj : objs)
				{
					if (obj->GetMaterial()->GetOwnerMaterial()->GetMaterialProperty().blendMode == GE::BlendMode::Transparent)
					{
						data.transparentObjects.push_back(obj); continue;
					}
					GraphicsPipelineState pso;
					if (!GetPSO(obj, pso))
					{
						continue;
					}

					DXCommand::BindPipelineState(data.cmdKey, pso);
					obj->DrawCall(&data.cmdKey);
				}

				passDataPtr->passMap[PASS_FINAL]->Run(&data);
				return true;
			};

			lock_guard<shared_mutex> lock(m_SetUpMutex);
			m_SetUpDataMap[setUp.name] = setUp;
		});

		// PassFinal
		m_PassSetUpTaskGroup.run([&]
		{
			PassSetUpData setUp;
			setUp.name = PASS_FINAL;
			setUp.ownerRenderer = this;
			setUp.setUpFunc = [&](RenderTarget& renderTarget, GraphicsPipelineState& pso) -> bool
			{
				return true;
			};
			setUp.runFunc = [&](DX12_RenderingPass* pass, PassRunData& data) -> bool
			{
				PassData* passDataPtr = (PassData*)data.passData;
				auto cbCam = data.scene->GetBindedCamera()->GetCameraCB();
				pass->GetRTRef().Resize(cbCam.lensWidth, cbCam.lensHeight);

				// 
				auto InitPass = passDataPtr->passMap[PASS_ENTRY];
				pass->GetRTRef()[0] = InitPass->GetRTRef()[0];
				pass->GetRTRef().SetDepthStencilTexture(
					InitPass->GetRTRef().GetDepthStencilTexture());

				DXCommand::SetRenderTarget(data.cmdKey, pass->GetRTRef());

				// 투명 객체 그리기
				for (auto& obj : data.transparentObjects)
				{
					GraphicsPipelineState pso;
					if (!GetPSO(obj, pso))
					{
						continue;
					}
					DXCommand::BindPipelineState(data.cmdKey, pso);
					obj->DrawCall(&data.cmdKey);
				}
				// test
				return true;
			};
			lock_guard<shared_mutex> lock(m_SetUpMutex);
			m_SetUpDataMap[PASS_FINAL] = setUp;
		});




		m_PassSetUpTaskGroup.wait();
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
			pso.BindRootSignature(m_RootSig);
			pso.BindRenderTarget({ DXGI_FORMAT_R8G8B8A8_UNORM }, DXGI_FORMAT_D24_UNORM_S8_UINT);
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

	DX12_GraphicsRenderer::PassData* DX12_GraphicsRenderer::CreatePassData()
	{
		uint64_t bufferIndex = DXDevice::GetBufferIndex();
		{
			lock_guard<std::shared_mutex> lock(m_PassDataQueueMutex);
			if (!m_WaitingPassData.empty())
			{
				auto passData = m_WaitingPassData.front(); m_WaitingPassData.pop();
				return passData;
			}

		}
		{
			lock_guard<std::shared_mutex> lock(m_PassDataPoolMutex);
			auto passData = make_unique<PassData>();
			for (auto& setUp_pair : m_SetUpDataMap)
			{
				auto pass = make_unique<DX12_RenderingPass>();
				auto passPtr = pass.get();
				passData->passPool.push_back(move(pass));
				passData->passMap[setUp_pair.first] = passPtr;

				if (!passPtr->SetUp((void*)&setUp_pair.second))
					return nullptr;
			}
			auto passDataPtr = passData.get();

			m_PassDataPool.push_back(move(passData));
			return passDataPtr;
		}
		

		return nullptr;
	}

}