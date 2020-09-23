#include "pch.h"
#include "GraphicPasses.h"
#include "../GraphicsIF.h"
using namespace std;
#define CASCADESHADOW_MAXCOUNT 4

namespace DX12
{
	std::string Entry::ShaderPath;
	std::string GBuffer::ShaderPath;
	std::string Lighting::ShaderPath;
	std::string ShadowMap::ShaderPath;
	/*
		SetUp = [](RenderTarget& rt, GraphicsPipelineState& pso, std::wstring& errCode)
		{

		};

		Run = [](GPRunData& data, std::wstring& error)
		{

		};
	*/

	Entry::Entry()
	{

		SetUp = [](RenderTarget& rt, GraphicsPipelineState& pso, std::wstring& errCode)
		{

		};

		Run = [](GPRunData& data, std::wstring& error)
		{
			// 그림자
			if(data.Scene->GetRendererMode() == GE::RendererMode::_3D)
			{
				data.Pass = data.GetPass<ShadowMap>();
				data.CmdKey = DXDevice::RequestGraphicsCommandKey();
				data.DrawingObjects = data.Scene->GetPushedObjects();
				DXCommand::BindRootSignature(data.CmdKey, GraphicPassMap::GetRootSig());
				data.Pass->Run(data, error);

			}


			// 본대
			{
				data.Pass = data.GetPass<GBuffer>();
				data.CmdKey = DXDevice::RequestGraphicsCommandKey();
				data.DrawingObjects = data.Scene->GetPushedObjects();
				DXCommand::BindRootSignature(data.CmdKey, GraphicPassMap::GetRootSig());
				data.Pass->Run(data, error);
			}
		};
	}


	GBuffer::GBuffer()
	{
		SetUp = [](RenderTarget& rt, GraphicsPipelineState& pso, std::wstring& errCode)
		{
			
			rt[0].CreateRenderTargetTexture(TT("Albedo"), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, 1);
			rt[1].CreateRenderTargetTexture(TT("Normal"), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, 1);
			rt.GetDepthStencilTexture().CreateDepthStencilTexture(
				TT("DepthStencil"), DXGI_FORMAT_D24_UNORM_S8_UINT, 1, 1, 1);
		};

		Run = [](GPRunData& data, std::wstring& error)
		{
			auto Cam = data.Scene->GetBindedCamera();
			auto& RT = data.Pass->RT;
			if (Cam)
			{
				CBCamera CamCB = Cam->GetCameraCB();
				RT.Resize(CamCB.lensWidth, CamCB.lensHeight);

				DXCommand::SetViewport(data.CmdKey, Viewport(CamCB.lensWidth, CamCB.lensHeight));
				DXCommand::SetScissorRect(data.CmdKey, ScissorRect(CamCB.lensWidth, CamCB.lensHeight));

				DXCommand::BindDynamicConstantBuffer(
					data.CmdKey, RootParam::Camera, &CamCB, sizeof(CamCB));

			}
			else error += TT("카메라가 존재 하지 않습니다. \n");
	
			DXCommand::ClearRenderTarget(data.CmdKey, RT);
			DXCommand::SetRenderTarget(data.CmdKey, RT);
			
			// 오브젝트 드로우 콜
			for (auto& object : data.DrawingObjects)
			{
				GraphicsPipelineState pso;
				if (!data.GetPSO(object, pso))
				{
					error += object->GetName() + TT(" : PSO 가져오기 실패 \n");
					continue;
				}
				
				DXCommand::BindPipelineState(data.CmdKey, pso);
				object->DrawCall(&data.CmdKey);
			}

			// 렌더러 모드에따른 분기점
			switch (data.Scene->GetRendererMode())
			{
			case GE::RendererMode::_3D:
				data.SetFinalPass<Lighting>(); 

				data.Pass = data.GetPass<Lighting>();
				data.Pass->Run(data, error);
				break;
			case GE::RendererMode::GUI:
			case GE::RendererMode::Paper:
				data.SetFinalPass<GBuffer>(); break;
			}
		};

	}


	Lighting::Lighting()
	{
		SetUp = [](RenderTarget& rt, GraphicsPipelineState& pso, std::wstring& errCode)
		{
			rt[0].CreateRenderTargetTexture(TT("Light"), DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, 1);

			VertexShader vShader = DX12::DXDevice::CreateVertexShaderFromFile(ShaderPath);
			PixelShader  pShader = DX12::DXDevice::CreatePixelShaderFromFile(ShaderPath);

			pso.BindRootSignature(GraphicPassMap::GetRootSig());
			pso.BindRenderTarget({ DXGI_FORMAT_R8G8B8A8_UNORM });
			{
				CD3DX12_DEPTH_STENCIL_DESC desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
				desc.DepthEnable = false;
				pso.SetDepthStencilState(desc);
			}

			pso.BindShader(ShaderType_Vertex, vShader);
			pso.BindShader(ShaderType_Pixel, pShader);
			if (!pso.Finalize())
			{
				errCode += TT("Lighting Pass SetUp 에 실패 \n");
			}
		};

		Run = [](GPRunData& data, std::wstring& error)
		{
			auto  Cam = data.Scene->GetBindedCamera();
			auto& RT  = data.Pass->RT;
			auto& PSO = data.Pass->PSO;
			if (Cam)
			{
				CBCamera CamCB = Cam->GetCameraCB();
				RT.Resize(CamCB.lensWidth, CamCB.lensHeight);

				DXCommand::SetViewport(data.CmdKey, Viewport(CamCB.lensWidth, CamCB.lensHeight));
				DXCommand::SetScissorRect(data.CmdKey, ScissorRect(CamCB.lensWidth, CamCB.lensHeight));
			}

			DXCommand::ClearRenderTarget(data.CmdKey, RT);
			DXCommand::SetRenderTarget(data.CmdKey, RT);
			DXCommand::BindPipelineState(data.CmdKey, PSO);

			std::vector<Texture> v;
			auto gbuffer = data.GetPass<GBuffer>();
			v.push_back(gbuffer->RT[0]);
			v.push_back(gbuffer->RT[1]);
			v.push_back(gbuffer->RT.GetDepthStencilTexture());
			std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC*> desc(v.size(), nullptr);

			DXCommand::BindTextures(data.CmdKey, RootParam::InputTexture,
				v.data(), (void**)desc.data(), v.size());

			SDPassData passData;
			passData.ShadowFilter = (uint32_t)GraphicsSetting::ShadowFilter;



			// 라이트
			auto dirLight = data.Scene->GetPushedDirectionalLight();
			if (dirLight)
			{
				passData.DirectionalLightCount = 1;
				auto& dlSDData = dirLight->GetSDData();
				DXCommand::BindDynamicConstantBuffer(data.CmdKey, RootParam::DirectionalLight,
					&dlSDData, sizeof(dlSDData));
				const uint32_t casCadeCount = std::min<float>(GraphicsSetting::CasCadeCount, CASCADESHADOW_MAXCOUNT);
				auto& t = (*((DX12::Texture*)dirLight->GetShadowMap()->GetUserTextureData()));
				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.ArraySize = casCadeCount;
				srvDesc.Texture2DArray.FirstArraySlice = 0;
				srvDesc.Texture2DArray.MipLevels = 1;
				srvDesc.Texture2DArray.MostDetailedMip = 0;
				srvDesc.Texture2DArray.PlaneSlice = 0;
				srvDesc.Texture2DArray.ResourceMinLODClamp = 0;

				DXCommand::BindTexture(data.CmdKey, RootParam::DLShadowMap, t, (void*)&srvDesc);

			}
			auto pointLights = data.Scene->GetPushedPointLights();
			if (!pointLights.empty())
			{
				uint32_t plCnt = pointLights.size();
				std::vector<SDPointLight> plDatas;
				std::vector<DX12::Texture>                    plShadowMaps;
				std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC*> plShadowDescs;
				plDatas.resize(plCnt); plShadowMaps.resize(plCnt);  plShadowDescs.resize(plCnt, nullptr);

				for (uint32_t i = 0; i < plCnt; ++i) 
				{
					plDatas[i] = pointLights[i]->GetSDData();
					plShadowMaps[i] = *((DX12::Texture*)(pointLights[i]->GetShadowCubeMap()->GetUserTextureData()));

					plShadowDescs[i] = new D3D12_SHADER_RESOURCE_VIEW_DESC();
					ZeroMemory(plShadowDescs[i], sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));

					plShadowDescs[i]->Format = DXGI_FORMAT_R32_FLOAT;
					plShadowDescs[i]->Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					plShadowDescs[i]->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					plShadowDescs[i]->TextureCube.MipLevels = 1;
				}
				DXCommand::BindDynamicStructuredBuffer(data.CmdKey, RootParam::PointLight,
					plDatas.data(), sizeof(SDPointLight), plCnt);
				DXCommand::BindTextures(data.CmdKey, RootParam::PLShadowMap, plShadowMaps.data(), (void**)plShadowDescs.data(),
					plCnt);

				for (uint32_t i = 0; i < plCnt; ++i)
				{
					delete plShadowDescs[i];
					plShadowDescs[i] = nullptr;
				}
				passData.PointLightCount = plCnt;
			}
			auto spotLights = data.Scene->GetPushedSpotLights();
			if (!spotLights.empty())
			{
				uint32_t slCnt = spotLights.size();
				std::vector<SDSpotLight> slDatas;
				std::vector<DX12::Texture>                   slShadowMaps;
				std::vector<D3D12_SHADER_RESOURCE_VIEW_DESC*> slShadowDescs;
				slDatas.resize(slCnt); slShadowMaps.resize(slCnt); slShadowDescs.resize(slCnt, nullptr);
				for (uint32_t i = 0; i < slCnt; ++i)
				{
					slDatas[i] = spotLights[i]->GetSDData();
					slShadowMaps[i] = *((DX12::Texture*)(spotLights[i]->GetShadowMap()->GetUserTextureData()));
				}
				DXCommand::BindDynamicStructuredBuffer(data.CmdKey, RootParam::SpotLight,
					slDatas.data(), sizeof(SDSpotLight), slCnt);
				DXCommand::BindTextures(data.CmdKey, RootParam::SLShadowMap, slShadowMaps.data(), (void**)slShadowDescs.data(), slCnt);

				passData.SpotLightCount = slCnt;
			}

			DXCommand::BindDynamicConstantBuffer(data.CmdKey, RootParam::PassData,
				&passData, sizeof(SDPassData));


			DXCommand::SetPrimitiveTopology(data.CmdKey, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			DXCommand::Draw(data.CmdKey, 6, 1);
		};
	}


	ShadowMap::ShadowMap()
	{
		SetUp = [](RenderTarget& rt, GraphicsPipelineState& pso, std::wstring& errCode)
		{
			InputLayout inputLayout;
			inputLayout.Add("POSITION", ShaderDataType::_float3_, 0);

			VertexShader vShader = DX12::DXDevice::CreateVertexShaderFromFile(ShadowMap::ShaderPath);
			PixelShader pShader = DX12::DXDevice::CreatePixelShaderFromFile(ShadowMap::ShaderPath);
			pso.BindRootSignature(GraphicPassMap::GetRootSig());
			pso.BindRenderTarget({}, DXGI_FORMAT_D32_FLOAT);
			pso.BindInputLayout(inputLayout);
			pso.BindShader(ShaderType_Vertex, vShader);
			pso.BindShader(ShaderType_Pixel, pShader);
			pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			if (!pso.Finalize())
			{
				errCode += TT("ShadowMap PSO 실패 \n");
			}
		};

		Run = [](GPRunData& data, std::wstring& error)
		{


			// 라이트 카메라
			auto& RT = data.Pass->RT;
			auto& PSO = data.Pass->PSO;

			JVector2 ShadowSize = GraphicsSetting::ShadowResolution;

			DXCommand::SetViewport(data.CmdKey, Viewport(ShadowSize.x, ShadowSize.y));
			DXCommand::SetScissorRect(data.CmdKey, ScissorRect(ShadowSize.x, ShadowSize.y));
			DXCommand::BindPipelineState(data.CmdKey, PSO);


			// DirectionalLight
			{
				GE::DirectionalLight*  dirLight = data.Scene->GetPushedDirectionalLight();
				if (dirLight && data.Scene->GetBindedCamera())
				{
					SDDirectionalLight DLData = dirLight->GetSDData();
					CBCamera sceneCamCB = data.Scene->GetBindedCamera()->GetCameraCB();
					GE::SceneCamera* sceneCam = data.Scene->GetBindedCamera();
					const uint32_t casCadeCount = std::min<float>(GraphicsSetting::CasCadeCount, CASCADESHADOW_MAXCOUNT);
					// 초기 데이터 셋팅
					float cascadeRange[casCadeCount + 1];
					cascadeRange[0] = sceneCamCB.nearZ;
					for (int i = 0; i < casCadeCount - 1; ++i)
					{
						cascadeRange[i + 1] = GraphicsSetting::CasCadeRange[i];
					}
					cascadeRange[casCadeCount] = sceneCamCB.farZ;

	
					JVector3 casCadeBoundCenter[casCadeCount];
					float    casCadeBoundRadius[casCadeCount];
					float casCadeOffsetX[casCadeCount];
					float casCadeOffsetY[casCadeCount];
					float cascadeScale[casCadeCount];
					JMatrix cascadeViewProj[casCadeCount];
					for (int i = 0; i < casCadeCount; ++i)
					{
						casCadeBoundRadius[i] = 0.0f;
					}
					float aspectRatio = sceneCamCB.lensWidth / sceneCamCB.lensHeight;
					// 함수 생성
					function<void(float, float, JVector3&, float&)> ExtractFrustomSphere =
						[&](float nearZ, float farZ, JVector3& boundCenter, float& r)
					{
						const float fTanFOVX = tanf(aspectRatio * sceneCam->GetFOV());
						const float fTanFOVY = tanf(aspectRatio);
						boundCenter = sceneCamCB.eyePos + sceneCam->GetLook() * (nearZ + 0.5f * (nearZ + farZ));
						const JVector3 boundSpan = sceneCamCB.eyePos + ((sceneCam->GetRight() * -1) * fTanFOVX + sceneCam->GetUp() * fTanFOVY + sceneCam->GetLook()) * farZ - boundCenter;
						r = JVector3::Length(boundSpan);
					};
					function<void(float, float, JVector3*)> ExtractFrustumPoints =
						[&](float nearZ, float farZ, JVector3* frustomConers)
					{
						const float fTanFOVX = tanf(aspectRatio * sceneCam->GetFOV());
						const float fTanFOVY = tanf(aspectRatio);
						frustomConers[0] = sceneCamCB.eyePos + ((sceneCam->GetRight() * -1) * fTanFOVX + sceneCam->GetUp() * fTanFOVY + sceneCam->GetLook()) * nearZ;
						frustomConers[1] = sceneCamCB.eyePos + (sceneCam->GetRight() * fTanFOVX + sceneCam->GetUp() * fTanFOVY + sceneCam->GetLook()) * nearZ;
						frustomConers[2] = sceneCamCB.eyePos + (sceneCam->GetRight() * fTanFOVX - sceneCam->GetUp() * fTanFOVY + sceneCam->GetLook()) * nearZ;
						frustomConers[3] = sceneCamCB.eyePos + ((sceneCam->GetRight() * -1) * fTanFOVX - sceneCam->GetUp() * fTanFOVY + sceneCam->GetLook()) * nearZ;
						frustomConers[4] = sceneCamCB.eyePos + ((sceneCam->GetRight() * -1) * fTanFOVX + sceneCam->GetUp() * fTanFOVY + sceneCam->GetLook()) * farZ;
						frustomConers[5] = sceneCamCB.eyePos + (sceneCam->GetRight() * fTanFOVX + sceneCam->GetUp() * fTanFOVY + sceneCam->GetLook()) * farZ;
						frustomConers[6] = sceneCamCB.eyePos + (sceneCam->GetRight() * fTanFOVX - sceneCam->GetUp() * fTanFOVY + sceneCam->GetLook()) * farZ;
						frustomConers[7] = sceneCamCB.eyePos + ((sceneCam->GetRight() * -1) * fTanFOVX - sceneCam->GetUp() * fTanFOVY + sceneCam->GetLook()) * farZ;

					};

					// 전체 라이트 뷰 매트릭스 생성
					JVector3 worldCenter = sceneCamCB.eyePos + sceneCam->GetLook() * sceneCamCB.farZ * 0.5f;
					JVector3 lookAt = worldCenter + (DLData.dir * sceneCamCB.farZ);
					JVector3 up = JVector3::Normalize(JVector3::Cross(DLData.dir, JVector3(1, 0, 0)));
					JMatrix  lightView = JMatrix::LookAtLH(worldCenter, lookAt, up);

					float ShadowBoundRadius = 0.0f;


					float fRadius = 0.0f;
					JVector3 boundCenter;
					ExtractFrustomSphere(sceneCamCB.nearZ, sceneCamCB.farZ, boundCenter, fRadius);
					ShadowBoundRadius = fRadius;


					JMatrix lightProj = JMatrix::OrthographicLH(ShadowBoundRadius, ShadowBoundRadius, -ShadowBoundRadius, ShadowBoundRadius);
					JMatrix lightViewProj = lightView * lightProj;
					dirLight->GetSDDataRef().viewProj = JMatrix::Transpose(lightViewProj);

					JMatrix lightInvView = JMatrix::Inverse(lightView);

					// 케스 케이드 별 데이터 갱신
					for (uint32_t i = 0; i < casCadeCount; ++i)
					{
						JMatrix cascadeTrans;
						JMatrix cascadeScales;

						JVector3 frustomPoints[8];
						ExtractFrustumPoints(cascadeRange[i], cascadeRange[i + 1], frustomPoints);
						JVector3 vMin = JVector3(FLT_MAX, FLT_MAX, FLT_MAX);
						JVector3 vMax = JVector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
						for (int i = 0; i < 8; i++)
						{
							JVector3 PointInShadowSpace;
							PointInShadowSpace = lightViewProj.TransformPoint(frustomPoints[i]);

							for (int j = 0; j < 3; j++)
							{
								if (vMin[j] > PointInShadowSpace[j])
									vMin[j] = PointInShadowSpace[j];
								if (vMax[j] < PointInShadowSpace[j])
									vMax[j] = PointInShadowSpace[j];
							}
						}
						JVector3 CascadeCenterShadowSpace = 0.5f * (vMin + vMax);

						// Update the translation from shadow to cascade space
						casCadeOffsetX[i] = -CascadeCenterShadowSpace.x;
						casCadeOffsetY[i] = -CascadeCenterShadowSpace.y;

						cascadeTrans = JMatrix::Translation(JVector3(casCadeOffsetX[i], casCadeOffsetY[i], 0.0f));
	
						cascadeScale[i] = 2.0f / max<float>(vMax.x - vMin.x, vMax.y - vMin.y);
						cascadeScales = JMatrix::Scaling(JVector3(cascadeScale[i], cascadeScale[i], 1.0f));
						cascadeViewProj[i] = lightViewProj * cascadeTrans * cascadeScales;
						dirLight->GetSDDataRef().cascadeOffsetX[i] = casCadeOffsetX[i];
						dirLight->GetSDDataRef().cascadeOffsetY[i] = casCadeOffsetY[i];
						dirLight->GetSDDataRef().cascadeScale[i] = cascadeScale[i];
					}

					for (uint32_t i = casCadeCount; i < CASCADESHADOW_MAXCOUNT; ++i)
					{
						dirLight->GetSDDataRef().cascadeOffsetX[i] = 250.0f;
						dirLight->GetSDDataRef().cascadeOffsetY[i] = 250.0f;
						dirLight->GetSDDataRef().cascadeScale[i] = 0.1f;
					}


					dirLight->GetSDDataRef().depthBias = GraphicsSetting::CasCadeDepthBias;
					auto t = (DX12::Texture*)dirLight->GetShadowMap()->GetUserTextureData();
					if (!t->IsValid())
					{
						t->CreateDepthStencilTexture(TT("ShadowMap"), DXGI_FORMAT_D32_FLOAT,
							ShadowSize.x, ShadowSize.y, 1, casCadeCount);
					}
					t->Resize(ShadowSize.x, ShadowSize.y);
					RT.GetDepthStencilTexture() = *t;
					D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

					dsvDesc.Format        = DXGI_FORMAT_D32_FLOAT;
					dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
					dsvDesc.Texture2DArray.ArraySize = 1;
					dsvDesc.Texture2DArray.MipSlice = 0;
					//CasCasedMatrix
					for (uint32_t i = 0; i < casCadeCount; ++i)
					{
					
						dsvDesc.Texture2DArray.FirstArraySlice = i;
						RT.SetDSVDesc(dsvDesc);


						CBCamera CamCB;
						CamCB.viewProj = JMatrix::Transpose(cascadeViewProj[i]);
						DXCommand::BindDynamicConstantBuffer(
							data.CmdKey, RootParam::Camera, &CamCB, sizeof(CamCB));

						DXCommand::ClearRenderTarget(data.CmdKey, RT, D3D12_CLEAR_FLAG_DEPTH);
						DXCommand::SetRenderTarget(data.CmdKey, RT);
						for (auto& object : data.DrawingObjects)
						{
							object->DrawCall(&data.CmdKey, true);
						}

					}

					RT.SetNullDSVDesc();

				}
			
			}



			// PointLight
			{
				auto pointLights = data.Scene->GetPushedPointLights();
				for (auto& l : pointLights)
				{
					auto t = (DX12::Texture*)l->GetShadowCubeMap()->GetUserTextureData();
					if (!t->IsValid())
					{
						t->CreateDepthStencilTexture(TT("ShadowMap"), DXGI_FORMAT_D32_FLOAT,
							ShadowSize.x, ShadowSize.y, 1, 6);
					}
					t->Resize(ShadowSize.x, ShadowSize.y);
					RT.GetDepthStencilTexture() = *t;

					D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

					dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
					dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
					dsvDesc.Texture2DArray.ArraySize = 1;
					dsvDesc.Texture2DArray.MipSlice = 0;

					for (uint32_t i = 0; i < 6; ++i)
					{
						dsvDesc.Texture2DArray.FirstArraySlice = i;
						RT.SetDSVDesc(dsvDesc);


						// 카메라 바인딩
						CBCamera CamCB;
						CamCB.viewProj = l->GetSDDataRef().viewProj[i];
						DXCommand::BindDynamicConstantBuffer(
							data.CmdKey, RootParam::Camera, &CamCB, sizeof(CamCB));

						// 렌더타겟 설정
						DXCommand::ClearRenderTarget(data.CmdKey, RT, D3D12_CLEAR_FLAG_DEPTH);
						DXCommand::SetRenderTarget(data.CmdKey, RT);

						for (auto& object : data.DrawingObjects)
						{
							object->DrawCall(&data.CmdKey, true);
						}

					}
					RT.SetNullDSVDesc();
				}
			}




			// SpotLight
			{
				auto spotLights = data.Scene->GetPushedSpotLights();
				for (auto& l : spotLights)
				{
					// 카메라 바인딩
					CBCamera CamCB;
					CamCB.viewProj = l->GetSDDataRef().viewProj;

					DXCommand::BindDynamicConstantBuffer(
						data.CmdKey, RootParam::Camera, &CamCB, sizeof(CamCB));

					// 렌더 타겟 설정
					auto t = (DX12::Texture*)l->GetShadowMap()->GetUserTextureData();
					if (!t->IsValid())
					{
						t->CreateDepthStencilTexture(TT("ShadowMap"),
							DXGI_FORMAT_D32_FLOAT, ShadowSize.x, ShadowSize.y, 1);
					}
					t->Resize(ShadowSize.x, ShadowSize.y);
					RT.GetDepthStencilTexture() = *t;

					DXCommand::ClearRenderTarget(data.CmdKey, RT, D3D12_CLEAR_FLAG_DEPTH);
					DXCommand::SetRenderTarget(data.CmdKey, RT);


					// DrawCall PosH
					for (auto& object : data.DrawingObjects)
					{
						object->DrawCall(&data.CmdKey, true);
					}
				}
			}

		};

	}

}