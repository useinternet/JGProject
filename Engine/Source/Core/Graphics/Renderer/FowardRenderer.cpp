#include "pch.h"
#include "FowardRenderer.h"
#include "Graphics/JGGraphics.h"

namespace JG
{

	FowardRenderer::FowardRenderer()
	{
		AddDrawFunc(
			std::bind(&FowardRenderer::Ready, this, std::placeholders::_1, std::placeholders::_2),
			std::bind(&FowardRenderer::Draw, this, std::placeholders::_1, std::placeholders::_2),
			nullptr);
	}

	int FowardRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}

	void FowardRenderer::Ready(IGraphicsAPI* api, const RenderInfo& info)
	{
		api->SetViewports(GetCommandID(), { Viewport(info.Resolutoin.x, info.Resolutoin.y) });
		api->SetScissorRects(GetCommandID(), { ScissorRect(0,0, info.Resolutoin.x,info.Resolutoin.y) });
		api->ClearRenderTarget(GetCommandID(), { info.TargetTexture }, info.TargetDepthTexture);
		api->SetRenderTarget(GetCommandID(), { info.TargetTexture }, info.TargetDepthTexture);
	}

	void FowardRenderer::Draw(int objectType, const List<ObjectInfo>& objectList)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		auto renderInfo = GetRenderInfo();
		auto lightInfos = GetLightInfos();


		struct PassCB
		{
			JMatrix ViewProjMatrix;
			JMatrix WorldMatrix;
			JVector3 EyePos;
		};
		PassCB passCB;
		passCB.ViewProjMatrix = JMatrix::Transpose(renderInfo.ViewProj);
		passCB.EyePos = renderInfo.EyePosition;



		for (auto& info : objectList)
		{
			auto mesh = info.Mesh;
			auto& materialList = info.MaterialList;
			auto& worldMatrix = info.WorldMatrix;

			if (mesh->Bind(GetCommandID()) == false)
			{
				JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetName());
			}
			for (u64 i = 0; i < mesh->GetSubMeshCount(); ++i)
			{
				if (mesh->GetSubMesh(i)->Bind(GetCommandID()) == false)
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
				material->SetPassData(GetCommandID(), &passCB, sizeof(PassCB));
				// 라이트 정보 심어주기
				{
					// 
					auto pointLightInfo = lightInfos[Graphics::ELightType::PointLight];
					if (material->SetInt(ShaderScript::Standard3D::PointLightCount, pointLightInfo.Count) == false)
					{

					}
					if (pointLightInfo.Count > 0)
					{
						if (material->SetStructDataArray(ShaderScript::Standard3D::PointLightList, pointLightInfo.ByteData.data(), pointLightInfo.Count, pointLightInfo.Size) == false)
						{

						}
					}
				}
				if (material->Bind(GetCommandID()) == false)
				{
					JG_CORE_INFO("{0} : Fail Material Bind", material->GetName());
					continue;
				}
				api->DrawIndexed(GetCommandID(), mesh->GetSubMesh(i)->GetIndexCount(), mesh->GetSubMesh(i)->GetInstanceCount());
			}
		}
	}

}