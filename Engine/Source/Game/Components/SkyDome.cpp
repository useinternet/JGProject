#include "pch.h"
#include "SkyDome.h"
#include "Class/Asset/Asset.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "GameWorld.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
namespace JG
{
	SkyDome::~SkyDome()
	{

	}
	void SkyDome::Awake()
	{
		GameComponent::Awake();
		CreateGeometry();
		auto shader = ShaderLibrary::GetInstance().GetShader(ShaderScript::Template::Standard3DShader, { "Surface/SkyDome" });
		mMaterial = IMaterial::Create("SkyDome_Material", shader);
		mMaterial->SetFloat3("ApexColor", JVector3(0.0f, 0.05f, 0.6f));
		mMaterial->SetFloat3("CenterColor", JVector3(0.0f, 0.5f, 0.8f));
		mMaterial->SetDepthStencilState(EDepthStencilStateTemplate::LessEqual);
		mMaterial->SetRasterizerState(ERasterizerStateTemplate::Cull_None);
		mPushRenderSceneObjectScheduleHandle = Scheduler::GetInstance().ScheduleByFrame(0, 0, -1, SchedulePriority::Graphics_PushSceneObject, SCHEDULE_BIND_FN(&SkyDome::PushRenderSceneObject));
	}
	void SkyDome::Start()
	{
		GameComponent::Start();
	}
	void SkyDome::Destory()
	{
		GameComponent::Destory();
		if (mPushRenderSceneObjectScheduleHandle != nullptr)
		{
			mPushRenderSceneObjectScheduleHandle->Reset();
		}
	}
	void SkyDome::Update()
	{
		GameComponent::Update();
	}

	void SkyDome::LateUpdate()
	{
		GameComponent::LateUpdate();
	}

	void SkyDome::MakeJson(SharedPtr<JsonData> jsonData) const
	{
		GameComponent::MakeJson(jsonData);
	}

	void SkyDome::LoadJson(SharedPtr<JsonData> jsonData)
	{
		GameComponent::LoadJson(jsonData);




	}

	//void SkyDome::OnInspectorGUI()
	//{
	//	GameComponent::OnInspectorGUI();

	//}

	EScheduleResult SkyDome::PushRenderSceneObject()
	{
		if (mMaterial == nullptr || (mMesh == nullptr || mMesh->IsValid() == false))
			return EScheduleResult::Continue;

		auto sceneObject = CreateSharedPtr<Graphics::StaticRenderObject>();
		JVector3 radius;
		JVector3 location;
		auto mainCam = Camera::GetMainCamera();
		if (mainCam)
		{
			f32 farZ = mainCam->GetFarZ() * 0.4f;
			radius = JVector3(farZ, farZ, farZ);
			location = mainCam->GetOwner()->GetTransform()->GetWorldLocation();
		}
		sceneObject->WorldMatrix = JMatrix::Scaling(radius) * JMatrix::Translation(location);
		sceneObject->MaterialList.push_back(mMaterial);
		sceneObject->Mesh = mMesh;
		GetGameWorld()->PushRenderSceneObject(sceneObject);
		return EScheduleResult::Continue;
	}

	void SkyDome::CreateGeometry()
	{
		List<JGVertex> vertices;
		List<u32> indicies;


		JGVertex topVertex;
		topVertex.Position = JVector3(0.0f, 1.0f, 0.0f);
		topVertex.Normal   = JVector3(0.0f, 1.0f, 0.0f);
		topVertex.Texcoord = JVector2(0.0f, 0.0f);
		JGVertex bottomVertex;
		bottomVertex.Position = JVector3(0.0f, -1.0f, 0.0f);
		bottomVertex.Normal = JVector3(0.0f, -1.0f, 0.0f);
		bottomVertex.Texcoord = JVector2(0.0f, 1.0f);


		u32 stackCount = 20.0f;
		u32 sliceCount = 20.0f;
		f32 phiStep   = JG_PI / (f32)stackCount;
		f32 thetaStep = 2.0f * JG_PI / (f32)sliceCount;


		vertices.push_back(topVertex);
		for (u32 i = 1; i <= stackCount - 1; ++i)
		{
			f32 phi = i * phiStep;

			for (u32 j = 0; j <= sliceCount; ++j)
			{
				f32 theta = j * thetaStep;

				JGVertex v;
				v.Position.x = sinf(phi) * cosf(theta);
				v.Position.y = cosf(phi);
				v.Position.z = sinf(phi) * sinf(theta);

				v.Normal = JVector3::Normalize(v.Position);
				v.Texcoord.x = theta / JG_2PI;
				v.Texcoord.y = phi / JG_PI;

				vertices.push_back(v);
			}
		}
		vertices.push_back(bottomVertex);


		for (u32 i = 1; i <= sliceCount; ++i)
		{
			indicies.push_back(0);
			indicies.push_back(i + 1);
			indicies.push_back(i);
		}
		u32 baseIndex = 1;
		u32 ringVertexCount = sliceCount + 1;
		for (u32 i = 0; i < stackCount - 2; ++i)
		{
			for (u32 j = 0; j < sliceCount; ++j)
			{
				indicies.push_back(baseIndex + i * ringVertexCount + j);
				indicies.push_back(baseIndex + i * ringVertexCount + j + 1);
				indicies.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				indicies.push_back(baseIndex + (i + 1) * ringVertexCount + j);
				indicies.push_back(baseIndex + i * ringVertexCount + j + 1);
				indicies.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
			}
		}
		u32 southPoleIndex = (u32)vertices.size() - 1;
		baseIndex = southPoleIndex - ringVertexCount;

		for (u32 i = 0; i < sliceCount; ++i)
		{
			indicies.push_back(southPoleIndex);
			indicies.push_back(baseIndex + i);
			indicies.push_back(baseIndex + i + 1);
		}


		auto vBuffer = IVertexBuffer::Create("SkySphere_VertexBuffer", EBufferLoadMethod::CPULoad);
		auto iBuffer = IIndexBuffer::Create("SkySphere_IndexBuffer", EBufferLoadMethod::CPULoad);

		vBuffer->SetData(vertices.data(), sizeof(JGVertex), vertices.size());
		iBuffer->SetData(indicies.data(), indicies.size());

		auto subMesh = ISubMesh::Create("SkySphere");
		subMesh->SetVertexBuffer(vBuffer);
		subMesh->SetIndexBuffer(iBuffer);
		

		mMesh = IMesh::Create("SkySphere");
		mMesh->SetInputLayout(JGVertex::GetInputLayout());
		mMesh->AddMesh(subMesh);




	}

}