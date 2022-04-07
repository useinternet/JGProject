#include "pch.h"
#include "GraphicsHelper.h"
#include "JGGraphics.h"
#include "Graphics/SceneObject.h"
#include "Class/Asset/Asset.h"
#include "RayTracing/TopLevelAccelerationStructure.h"
#include "Shader.h"


namespace JG
{
	String GraphicsHelper::GetShaderTemplateName(SharedPtr<IGraphicsShader> shader)
	{
		if (shader == nullptr) {
			return "";
		}
		String name = shader->GetName();


		List<String> splitedName = StringHelper::Split(name, '/');

		if (splitedName.empty() == true)
		{
			return "";
		}

		return splitedName[0];
	}
	SharedPtr<Graphics::StaticRenderObject> GraphicsHelper::CreateSkyBox(const JVector3& eyePos, f32 farZ, const String& texturePath)
	{
		SharedPtr<Graphics::StaticRenderObject> sceneObject = CreateSharedPtr<Graphics::StaticRenderObject>();
		SharedPtr<IMesh> mesh;
		// Create Mesh
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


			mesh = IMesh::Create("SkySphere");
			mesh->SetInputLayout(JGVertex::GetInputLayout());
			mesh->AddMesh(subMesh);
		}
		SharedPtr<IMaterial> material;
		{
			auto shader = ShaderLibrary::GetInstance().FindGraphicsShader(ShaderDefine::Template::Standard3DShader, { "Surface/SkyDome" });
			material = IMaterial::Create("SkyDome_Material", shader);
			material->SetFloat3("ApexColor", JVector3(0.0f, 0.05f, 0.6f));
			material->SetFloat3("CenterColor", JVector3(0.0f, 0.5f, 0.8f));
			material->SetDepthStencilState(EDepthStencilStateTemplate::LessEqual);
			material->SetRasterizerState(ERasterizerStateTemplate::Cull_None);
		}
		SharedPtr<ITexture> cubeTex;
		{
			SharedPtr<Asset<ITexture>> cubeTexAsset = AssetDataBase::GetInstance().LoadOriginAsset<ITexture>("Asset/Engine/CubeMap/DefaultSky.jgasset");
			if (cubeTexAsset != nullptr && cubeTexAsset->IsValid())
			{
				cubeTex = cubeTexAsset->Get();
			}
		}
		if (material != nullptr)
		{
			material->SetTexture("Tex1", cubeTex);
		}
		sceneObject->Mesh = mesh;
		sceneObject->MaterialList.push_back(material);
		sceneObject->Flags = Graphics::ESceneObjectFlags::No_Shadow;
		sceneObject->WorldMatrix = JMatrix::Scaling(JVector3(farZ, farZ, farZ)) * JMatrix::Translation(eyePos);


		return sceneObject;
	}
	void GraphicsHelper::InitTopLevelAccelerationStructure(List<SharedPtr<ITopLevelAccelerationStructure>>* out_as_list)
	{
		if (out_as_list == nullptr)
		{
			return;
		}


		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();

		List<SharedPtr<ITopLevelAccelerationStructure>>& as_list = *out_as_list;
		as_list.resize(bufferCnt);

		i32 index = 0;
		for (auto& as : as_list)
		{
			as = ITopLevelAccelerationStructure::Create();
		}
	}
	void GraphicsHelper::InitRenderTextures(const TextureInfo& info, const String& name,  List<SharedPtr<ITexture>>* out_t_list)
	{
		if (out_t_list == nullptr)
		{
			return;
		}


		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();

		List<SharedPtr<ITexture>>& t_list = *out_t_list;
		t_list.resize(bufferCnt);

		i32 index = 0;
		for (auto& t : t_list)
		{
			if (t == nullptr) t = ITexture::Create(name + "_" + std::to_string(index), info);
			else t->SetTextureInfo(info);

			++index;
		}
	}

	void GraphicsHelper::InitByteAddressBuffer(const String& name, u64 elementCount, List<SharedPtr<IByteAddressBuffer>>* out_b_list)
	{
		if (out_b_list == nullptr)
		{
			return;
		}

		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();

		List<SharedPtr<IByteAddressBuffer>>& b_list = *out_b_list;
		b_list.resize(bufferCnt);

		i32 index = 0;
		for (auto& b : b_list)
		{
			b = IByteAddressBuffer::Create(name + "_" + std::to_string(index), elementCount);
			++index;
		}
	}

	void GraphicsHelper::InitStrucutredBuffer(const String& name, u64 elementCount, u64 elementSize, List<SharedPtr<IStructuredBuffer>>* out_sb_list, EBufferLoadMethod method)
	{
		if (out_sb_list == nullptr)
		{
			return;
		}
		u32 bufferCnt = JGGraphics::GetInstance().GetBufferCount();

		List<SharedPtr<IStructuredBuffer>>& sb_list = *out_sb_list;
		sb_list.resize(bufferCnt);


		i32 index = 0;
		for (auto& sb : sb_list)
		{
			sb = IStructuredBuffer::Create(name + "_" + std::to_string(index), elementSize, elementCount, method);
			++index;
		}
	}
	u32 GraphicsHelper::NumMantissaBitsInFloatFormat(u32 FloatFormatBitLength)
	{
		switch (FloatFormatBitLength)
		{
		case 32: return 23;
		case 16: return 10;
		case 11: return 6;
		case 10: return 5;
		}

		return 0;
	}
}