#include "pch.h"
#include "StaticMeshComponent.h"
#include "GFWClasses/GameObject.h"
#include "GFWClasses/World.h"
#include "GFWIF.h"

using namespace std;

namespace GFW
{



	void StaticMeshComponent::Awake()
	{
		MeshComponent::Awake();
	}

	void StaticMeshComponent::Start()
	{
		MeshComponent::Start();
	}

	void StaticMeshComponent::Tick()
	{
		MeshComponent::Tick();


		m_Instance.sdData.world = GetWorldMatrix();
		GetWorld()->PushSceneObjects(m_Instance.objects);

		

	}

	void StaticMeshComponent::Destroy()
	{
		MeshComponent::Destroy();
	}

	void StaticMeshComponent::SetMesh(const std::vector<MeshGenerator::StaticMeshData>& meshData)
	{
		size_t meshCnt = meshData.size();
		GFWIF::UnRegisterInstanceWithGraphicsEngine(this);
		GFWIF::RegisterInstanceWithGraphicsEngine(this);

		m_Instance.objects.resize(meshCnt);
		m_Instance.meshDatas = meshData;


		for(size_t i = 0; i<meshCnt; ++i)
		{
			m_Instance.objects[i] = GFWIF::RequestSceneObjectWithGraphicsEngine(this);
			m_Instance.objects[i]->BindMesh(
				m_Instance.meshDatas[i].v.data(), m_Instance.meshDatas[i].v.size(),
				m_Instance.meshDatas[i].i.data(), m_Instance.meshDatas[i].i.size()
			);
			m_Instance.objects[i]->BindMaterial(nullptr);
			m_Instance.objects[i]->BindInstanceData(&m_Instance.sdData, 1);
		}
	}

}