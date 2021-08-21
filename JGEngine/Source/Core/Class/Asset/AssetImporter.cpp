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
			aiProcess_JoinIdenticalVertices |     // 동일한 꼭지점 결합, 인덱싱 최적화
			aiProcess_ValidateDataStructure |     // 로더의 출력을 검증
			aiProcess_ImproveCacheLocality |      // 출력 정점의 캐쉬위치를 개선
			aiProcess_RemoveRedundantMaterials |  // 중복된 매터리얼 제거
			aiProcess_GenUVCoords |               // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
			aiProcess_TransformUVCoords |         // UV 변환 처리기 (스케일링, 변환...)
			aiProcess_FindInstances |             // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
			aiProcess_LimitBoneWeights |          // 정점당 뼈의 가중치를 최대 4개로 제한
			aiProcess_OptimizeMeshes |            // 가능한 경우 작은 매쉬를 조인
			aiProcess_GenSmoothNormals |          // 부드러운 노말벡터(법선벡터) 생성
			aiProcess_SplitLargeMeshes |          // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
			aiProcess_Triangulate |               // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
			aiProcess_ConvertToLeftHanded |       // D3D의 왼손좌표계로 변환
			aiProcess_SortByPType |               // 단일타입의  프리미티브로 구성된 '깨끗한' 매쉬를 만듬
			aiProcess_CalcTangentSpace            // 탄젠트 공간 계산 )
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

			int multipleTex = 0;
			int texCount = 0;
			while (mesh->HasTextureCoords(texCount) == true)
			{
				texCount++;
				multipleTex++;
			}
			if (multipleTex >= 2)
			{
				JG_CORE_ERROR("This Mesh is multiple texcoord : {0}", output->Name);
			}


			if (mesh->HasTextureCoords(0))
			{
				auto& ai_tex = mesh->mTextureCoords[0][i];
				v.Texcoord.x = ai_tex.x;
				v.Texcoord.y = ai_tex.y;
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
		if (MaterialAssetStock::Write(outputPath, stock) == false)
		{
			JG_CORE_ERROR("Fail Write Material : {0} ", outputPath);
		}
	}

}