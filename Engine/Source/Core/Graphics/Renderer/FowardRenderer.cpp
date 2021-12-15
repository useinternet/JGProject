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
		auto commandID = JGGraphics::GetInstance().RequestCommandID();
		if (mComputeCluster)
		{
			mComputeCluster->CB.InvProjMatrix = JMatrix::Transpose(JMatrix::Inverse(info.ProjMatrix));
			mComputeCluster->CB.ViewMatrix = JMatrix::Transpose(info.ViewMatrix);
			mComputeCluster->CB.EyePosition = info.EyePosition;
			mComputeCluster->CB.Resolution = info.Resolutoin;
			mComputeCluster->CB.FarZ  = info.FarZ;
			mComputeCluster->CB.NearZ = info.NearZ;
			mComputeCluster->CB.PointLightCount = GetLightInfo(Graphics::ELightType::PointLight).Count;
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
		
		ForEach([&](int objectType, const List<ObjectInfo>& objectList)
		{
			for (auto& info : objectList)
			{
				auto mesh = info.Mesh;
				auto& materialList = info.MaterialList;
				auto& worldMatrix  = JMatrix::Transpose(info.WorldMatrix);
				api->SetTransform(commandID, &worldMatrix);

				if (mesh->Bind(commandID) == false)
				{
					JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetName());
				}
				for (u64 i = 0; i < mesh->GetSubMeshCount(); ++i)
				{
					if (mesh->GetSubMesh(i)->Bind(commandID) == false)
					{
						JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetSubMesh(i)->GetName());
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

	void FowardRenderer::CompeleteImpl(IGraphicsAPI* api, const RenderInfo& info)
	{


	}

	int FowardRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}


}