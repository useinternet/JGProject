#include "pch.h"

#include "AssetImporter.h"
#include "Application.h"
#include "Graphics/Shader.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"


namespace JG
{
	EAssetImportResult AssetImporter::Import(const FBXAssetImportSettings& setting)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(setting.AssetPath,
			aiProcess_JoinIdenticalVertices |     // ������ ������ ����, �ε��� ����ȭ
			aiProcess_ValidateDataStructure |     // �δ��� ����� ����
			aiProcess_ImproveCacheLocality |      // ��� ������ ĳ����ġ�� ����
			aiProcess_RemoveRedundantMaterials |  // �ߺ��� ���͸��� ����
			aiProcess_GenUVCoords |               // ����, ������, ���� �� ��� ������ ������ UV�� ��ȯ
			aiProcess_TransformUVCoords |         // UV ��ȯ ó���� (�����ϸ�, ��ȯ...)
			aiProcess_FindInstances |             // �ν��Ͻ��� �Ž��� �˻��Ͽ� �ϳ��� �����Ϳ� ���� ������ ����
			aiProcess_LimitBoneWeights |          // ������ ���� ����ġ�� �ִ� 4���� ����
			aiProcess_OptimizeMeshes |            // ������ ��� ���� �Ž��� ����
			aiProcess_GenSmoothNormals |          // �ε巯�� �븻����(��������) ����
			aiProcess_SplitLargeMeshes |          // �Ŵ��� �ϳ��� �Ž��� �����Ž���� ��Ȱ(����)
			aiProcess_Triangulate |               // 3�� �̻��� �𼭸��� ���� �ٰ��� ���� �ﰢ������ ����(����)
			aiProcess_ConvertToLeftHanded |       // D3D�� �޼���ǥ��� ��ȯ
			aiProcess_SortByPType |               // ����Ÿ����  ������Ƽ��� ������ '������' �Ž��� ����
			aiProcess_CalcTangentSpace            // ź��Ʈ ���� ��� )
		);


		if (scene != nullptr)
		{
			StaticMeshAssetStock meshInfo;
			meshInfo.Name = scene->mName.C_Str();
			if (scene->HasMeshes() == true)
			{
				u32 meshCount = scene->mNumMeshes;
				for (u32 i = 0; i < meshCount; ++i)
				{
					auto mesh = scene->mMeshes[i];
					ReadMesh(mesh, &meshInfo);
				}
				WriteMesh(setting.OutputPath, meshInfo);
			}
			if (scene->HasAnimations() == true)
			{

			}
			if (scene->HasMaterials() == true)
			{

			}
			if (scene->HasTextures() == true)
			{

			}
			
		}





		return EAssetImportResult::Success;
	}
	EAssetImportResult AssetImporter::Import(const TextureAssetImportSettings& settings)
	{
		TextureAssetStock stock;
		
		auto fileName = fs::path(settings.AssetPath).filename().string();
		u64 extentionPos = fileName.find_last_of(".");
		stock.Name = fileName.substr(0, extentionPos);
		
		
		byte* pixels = stbi_load(settings.AssetPath.c_str(), &stock.Width, &stock.Height, &stock.Channels, STBI_rgb_alpha);
		if (pixels == nullptr)
		{
			return EAssetImportResult::Fail;
		}
		stock.Channels = 4;
		u64 size = (u64)stock.Width * (u64)stock.Height * (u64)stock.Channels;
		stock.Pixels.resize(size);
		memcpy(&stock.Pixels[0], pixels, size);


		stbi_image_free(pixels);

		WriteTexture(settings.OutputPath, stock);
		return EAssetImportResult::Success;
	}
	EAssetImportResult AssetImporter::Import(const MaterialAssetImportSettings& settings)
	{
		MaterialAssetStock stock;

		stock.Name           = settings.FileName;
		stock.ShaderTemplate = settings.Shader;
		stock.ShaderScript   = settings.ShaderScript;
		

		WriteMaterial(settings.OutputPath, stock);
		return EAssetImportResult::Success;
	}
	void AssetImporter::ReadMesh(aiMesh* mesh, StaticMeshAssetStock* output)
	{
		if (output == nullptr || mesh == nullptr)
		{
			return;
		}
		List<JGVertex> vertices;
		List<u32>   indices;
		output->SubMeshNames.push_back(mesh->mName.C_Str());

		vertices.resize(mesh->mNumVertices);

		for (u32 i = 0; i < mesh->mNumVertices; ++i)
		{
			JGVertex v;
			if (mesh->HasPositions() == true)
			{
				auto& ai_pos = mesh->mVertices[i];
				v.Position.x = ai_pos.x;
				v.Position.y = ai_pos.y;
				v.Position.z = ai_pos.z;

				for (i32 i = 0; i < 3; ++i)
				{
					output->BoundingBox.min[i] = std::min<float>(output->BoundingBox.min[i], v.Position[i]);
					output->BoundingBox.max[i] = std::max<float>(output->BoundingBox.max[i], v.Position[i]);
				}
			}

			if (mesh->HasNormals() == true)
			{
				auto& ai_nor = mesh->mNormals[i];

				v.Normal.x = ai_nor.x;
				v.Normal.y = ai_nor.y;
				v.Normal.z = ai_nor.z;
			}


			if (mesh->HasTangentsAndBitangents() == true)
			{
				auto& ai_tan = mesh->mTangents[i];
				v.Tangent.x = ai_tan.x;
				v.Tangent.y = ai_tan.y;
				v.Tangent.z = ai_tan.z;

				auto& ai_bit = mesh->mBitangents[i];
				v.Bitangent.x = ai_bit.x;
				v.Bitangent.y = ai_bit.y;
				v.Bitangent.z = ai_bit.z;
			}


			vertices[i] = v;
		}

		output->Vertices.push_back(vertices);

		if (mesh->HasFaces() == true)
		{
			for (u32 i = 0; i < mesh->mNumFaces; ++i)
			{
				auto& face = mesh->mFaces[i];

				for (u32 j = 0; j < face.mNumIndices; ++j)
				{
					indices.push_back(face.mIndices[j]);
				}
			}
		}
		output->Indices.push_back(indices);


		if (mesh->HasBones() == true)
		{

		}
	}
	void AssetImporter::WriteMesh(const String& outputPath, StaticMeshAssetStock& stock)
	{
		if (stock.Name.empty() == true)
		{
			stock.Name = stock.SubMeshNames[0];
		}

		auto filePath = CombinePath(outputPath, stock.Name) + JG_ASSET_FORMAT;
		auto json = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_FORMAT_KEY, (u64)EAssetFormat::Mesh);
		json->AddMember(JG_ASSET_KEY, stock);

		if (Json::Write(filePath, json) == false)
		{
			JG_CORE_ERROR("Fail Write Mesh : {0} ", outputPath);
		}
	}

	void AssetImporter::WriteTexture(const String& outputPath, TextureAssetStock& stock)
	{
		auto filePath = CombinePath(outputPath, stock.Name) + JG_ASSET_FORMAT;

		auto json = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_FORMAT_KEY, (u64)EAssetFormat::Texture);
		json->AddMember(JG_ASSET_KEY, stock);

		if (Json::Write(filePath, json) == false)
		{
			JG_CORE_ERROR("Fail Write Texture : {0} ", outputPath);
		}
	}

	void AssetImporter::WriteMaterial(const String& outputPath, MaterialAssetStock& stock)
	{
		auto filePath = CombinePath(outputPath, stock.Name) + JG_ASSET_FORMAT;

		auto json	  = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_FORMAT_KEY, (u64)EAssetFormat::Material);
		json->AddMember(JG_ASSET_KEY, stock);

		if (Json::Write(filePath, json) == false)
		{
			JG_CORE_ERROR("Fail Write Material : {0} ", outputPath);
		}
	}

}