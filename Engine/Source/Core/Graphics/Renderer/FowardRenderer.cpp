#include "pch.h"
#include "FowardRenderer.h"
#include "Graphics/JGGraphics.h"

namespace JG
{

	FowardRenderer::FowardRenderer()
	{
		PushDrawFunc(std::bind(&FowardRenderer::Draw, this, std::placeholders::_1, std::placeholders::_2));
	}

	int FowardRenderer::ArrangeObject(const ObjectInfo& info)
	{
		return 0;
	}

	void FowardRenderer::Draw(int objectType, const List<ObjectInfo>& objectList)
	{
		auto api = JGGraphics::GetInstance().GetGraphicsAPI();
		JGASSERT_IF(api != nullptr, "GraphicsApi is nullptr");

		auto renderInfo = GetRenderInfo();
		auto lightInfos = GetLightInfos();
		for (auto& info : objectList)
		{
			auto mesh = info.Mesh;
			auto& materialList = info.MaterialList;
			auto& worldMatrix = info.WorldMatrix;

			mesh->SetCommandID(GetCommandID());
			if (mesh->Bind() == false)
			{
				JG_CORE_ERROR("{0} : Fail Mesh Bind", mesh->GetName());
			}

			auto transposedViewProj = JMatrix::Transpose(renderInfo.ViewProj);
			for (u64 i = 0; i < mesh->GetSubMeshCount(); ++i)
			{
				mesh->GetSubMesh(i)->SetCommandID(GetCommandID());
				if (mesh->GetSubMesh(i)->Bind() == false)
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
				auto transposedWorld = JMatrix::Transpose(worldMatrix);
				if (material->SetFloat4x4(ShaderScript::Standard3D::ViewProj, transposedViewProj) == false)
				{
					JG_CORE_ERROR("{0} : Fail SetViewProjMatrix in CameraParam", material->GetName());
					continue;
				}
				if (material->SetFloat3(ShaderScript::Standard3D::Eye, renderInfo.EyePosition) == false)
				{
					JG_CORE_ERROR("{0} : Fail SetEye in CameraParam", material->GetName());
					continue;
				}
				if (material->SetFloat4x4(ShaderScript::Standard3D::World, transposedWorld) == false)
				{
					JG_CORE_ERROR("{0} : Fail SetWorldMatrix in ObjectParams", material->GetName());
					continue;
				}


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



				material->SetCommandID(GetCommandID());
				if (material->Bind() == false)
				{
					JG_CORE_INFO("{0} : Fail Material Bind", material->GetName());
					continue;
				}
				api->DrawIndexed(GetCommandID(), mesh->GetSubMesh(i)->GetIndexCount(), mesh->GetSubMesh(i)->GetInstanceCount());
			}
		}
	}

}