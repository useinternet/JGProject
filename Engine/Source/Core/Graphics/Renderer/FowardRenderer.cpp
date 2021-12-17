#include "pch.h"
#include "FowardRenderer.h"
#include "Graphics/JGGraphics.h"
#include "Graphics/PreRenderProcess/PreRenderProcess_ComputeCluster.h"
#include "Graphics/PreRenderProcess/PreRenderProcess_LightCulling.h"
namespace JG
{

	FowardRenderer::FowardRenderer()
	{
		AddPreProcess<PreRenderProcess_ComputeCluster>();
		AddPreProcess<PreRenderProcess_LightCulling>();

	}

	void FowardRenderer::ReadyImpl(IGraphicsAPI* api, Graphics::RenderPassData* renderPassData, const RenderInfo& info)
	{
		auto commandID = JGGraphics::GetInstance().RequestCommandID();

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

					if (material == nullptr || material->Bind(commandID) == false)
					{
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