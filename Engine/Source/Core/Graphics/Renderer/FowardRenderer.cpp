#include "pch.h"
#include "FowardRenderer.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/PreRenderProcess/PreRenderProcess_ComputeCluster.h"
namespace JG
{

	FowardRenderer::FowardRenderer()
	{
		mComputeCluster = AddPreProcess<PreRenderProcess_ComputeCluster>();

	}

	void FowardRenderer::ReadyImpl(IGraphicsAPI* api, const RenderInfo& info)
	{
		if (mComputeCluster)
		{
			mComputeCluster->CB.InvProjMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ProjMatrix));
			mComputeCluster->CB.EyePosition = info.EyePosition;
			mComputeCluster->CB.Resolution = info.Resolutoin;
			mComputeCluster->CB.FarZ  = info.FarZ;
			mComputeCluster->CB.NearZ = info.NearZ;
			mComputeCluster->CB.TileSize = JVector2(
				info.Resolutoin.x / (f32)PreRenderProcess_ComputeCluster::NUM_X_SLICE,
				info.Resolutoin.y / (f32)PreRenderProcess_ComputeCluster::NUM_Y_SLICE);


		}

	}

	void FowardRenderer::RenderImpl(IGraphicsAPI* api, const RenderInfo& info)
	{
		auto commandID = JGGraphics::GetInstance().RequestCommandID();
		api->SetViewports(commandID, { Viewport(info.Resolutoin.x, info.Resolutoin.y) });
		api->SetScissorRects(commandID, { ScissorRect(0,0, info.Resolutoin.x,info.Resolutoin.y) });
		api->ClearRenderTarget(commandID, { info.TargetTexture }, info.TargetDepthTexture);
		api->SetRenderTarget(commandID, { info.TargetTexture }, info.TargetDepthTexture);
		
		PassCB passCB;
		passCB.ViewProjMatrix = JMatrix::Transpose(info.ViewProjMatrix);
		passCB.EyePos = info.EyePosition;

		auto lightInfos = GetLightInfos();
		ForEach([&](int objectType, const List<ObjectInfo>& objectList)
		{
			for (auto& info : objectList)
			{
				auto mesh = info.Mesh;
				auto& materialList = info.MaterialList;
				auto& worldMatrix = info.WorldMatrix;

				if (mesh->Bind(commandID) == false)
				{
					JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetName());
				}
				for (u64 i = 0; i < mesh->GetSubMeshCount(); ++i)
				{
					if (mesh->GetSubMesh(i)->Bind(commandID) == false)
					{
						JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetSubMesh(0)->GetName());
						continue;
					}
					SharedPtr<IMaterial> material = nullptr;
					if (materialList.size() <= i)
					{
						material = materialList[0];
					}
					else
					{
						material = materialList[i];
					}
					passCB.WorldMatrix = JMatrix::Transpose(worldMatrix);
					material->SetPassData(commandID, &passCB, sizeof(PassCB));

					ForEach([&](Graphics::ELightType lightType, const LightInfo& lightInfo)
					{

						switch (lightType)
						{
						case Graphics::ELightType::PointLight:
						{
							
							if (material->SetInt(ShaderDefine::Standard3D::PointLightCount, lightInfo.Count) == false)
							{

							}
							if (lightInfo.Count > 0)
							{
								if (material->SetStructDataArray(ShaderDefine::Standard3D::PointLightList, (void*)lightInfo.ByteData.data(), lightInfo.Count, lightInfo.Size) == false)
								{

								}
							}
						}
							break;
						}
					});

					if (material->Bind(commandID) == false)
					{
						JG_CORE_INFO("{0} : Fail Material Bind", material->GetName());
						continue;
					}
					api->DrawIndexed(commandID, mesh->GetSubMesh(i)->GetIndexCount(), mesh->GetSubMesh(i)->GetInstanceCount());
				}
			}
		});

	}

	int FowardRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}


}