#include "pch.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "AssetImporter.h"
#include "Application.h"
#include "Graphics/Shader.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "zlib/zlib.h"



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
			if (scene->HasMeshes() == true && setting.Flags & EFBXAssetImportFlags::Import_Mesh)
			{
				StaticMeshAssetStock meshInfo;
				meshInfo.Name = scene->mName.C_Str();
				u32 meshCount = scene->mNumMeshes;
				for (u32 i = 0; i < meshCount; ++i)
				{
					auto mesh = scene->mMeshes[i];
					ReadMesh(mesh, &meshInfo);
				}
				WriteMesh(setting.OutputPath, meshInfo);
			}
			if (scene->HasMeshes() == true && setting.Flags & EFBXAssetImportFlags::Import_Skeletal)
			{
				u32 meshCount = scene->mNumMeshes;
				for (u32 i = 0; i < meshCount; ++i)
				{
					aiMesh* mesh = scene->mMeshes[i];
					if (mesh->HasBones() == true)
					{
						SkeletalAssetStock skeletalInfo;
						skeletalInfo.Name = mesh->mName.C_Str() + String("_Skeletal");
						ReadSkeletal(scene, mesh, &skeletalInfo);
						WriteSkeletal(setting.OutputPath, skeletalInfo);
					}
				}
			}
			if (scene->HasAnimations() == true && setting.Flags & EFBXAssetImportFlags::Import_AnimationClip)
			{

				u32 animCount = scene->mNumAnimations;
				for (u32 i = 0; i < animCount; ++i)
				{
					AnimationClipAssetStock animInfo;
					aiAnimation* anim = scene->mAnimations[i];
				
					ReadAnimation(anim, &animInfo);
					WriteAnimation(setting.OutputPath, animInfo);
				}
			}
			if (scene->HasMaterials() == true)
			{
				u32 materialCount = scene->mNumMaterials;
				for (u32 i = 0; i < materialCount; ++i)
				{
					MaterialAssetStock materialInfo;

					aiMaterial* mat = scene->mMaterials[i];

					ReadMaterial(mat, &materialInfo);

				}
				// Material Property
				// TextureIndex
			}
			if (scene->HasTextures() == true && setting.Flags & EFBXAssetImportFlags::Import_Texture)
			{
				// Texture
				u32 texCnt = scene->mNumTextures;
				for (u32 i = 0; i < texCnt; ++i)
				{
					TextureAssetStock texInfo;
					aiTexture* tex = scene->mTextures[i];
					ReadTexture(tex, &texInfo);
					WriteTexture(setting.OutputPath, texInfo);
				}
			}
			
		}
		else
		{
			JG_LOG_ERROR("Assimp Importer ReadFiles Error : {0}", importer.GetErrorString());
			return EAssetImportResult::Fail;
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
		stock.OriginPixelSize = size;
		stock.Pixels.resize(size);
		i32 result = compress((Bytef*)stock.Pixels.data(), (uLongf*)(&size), pixels, size);
		stock.Pixels.resize(size);
	

		stbi_image_free(pixels);
		if (result != Z_OK)
		{
			return EAssetImportResult::Fail;
		}
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
	EAssetImportResult AssetImporter::Import(const CubeMapAssetImportSettings& settings)
	{
		TextureAssetStock stock;
		i32 width    = 0.0f;
		i32 height   = 0.0f;
		i32 channels = 0;
		u64 btPos    = 0;
		List<jbyte> pixeldata;
		
		stock.Name = fs::path(settings.Name).filename().string();

		for (i32 i = 0; i < 6; ++i)
		{
			byte* pixels = stbi_load(settings.AssetPath[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);
			if (pixels == nullptr)
			{
				return EAssetImportResult::Fail;
			}
			stock.Channels = 4;
			u64 btSize = width * height * stock.Channels;

			pixeldata.resize(btPos + btSize);

			memcpy(&pixeldata[btPos], pixels, btSize);
			btPos += btSize;

			stbi_image_free(pixels);

			if (i > 0 && (stock.Width != width || stock.Height != height))
			{
				return EAssetImportResult::Fail;
			}
			stock.Width = width;
			stock.Height = height;
		}
		stock.OriginPixelSize = btPos;
		u64 btSize = btPos;

		stock.Pixels.resize(btSize);
		i32 result = compress((Bytef*)(stock.Pixels.data()), (uLongf*)(&btSize), (const Bytef*)pixeldata.data(), btSize);
		stock.Pixels.resize(btSize);

		if (result != Z_OK)
		{
			return EAssetImportResult::Fail;
		}
		
		WriteCubeMap(settings.OutputPath, stock);

		return EAssetImportResult::Success;
	}
	void AssetImporter::ReadMesh(const aiMesh* mesh, StaticMeshAssetStock* output)
	{
		if (output == nullptr || mesh == nullptr)
		{
			return;
		}
		List<JGVertex> vertices;
		List<JGBoneVertex> boneVertices;
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
				JG_LOG_ERROR("This Mesh is multiple texcoord : {0}", output->Name);
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
			boneVertices.resize(vertices.size());

			u32 boneCount = mesh->mNumBones;
			for (u32 i = 0; i < boneCount; ++i)
			{
				aiBone* bone = mesh->mBones[i];

				u32 weightCnt = bone->mNumWeights;
				for (u32 j = 0; j < weightCnt; ++j)
				{
					u32 vertexID	 = bone->mWeights[j].mVertexId;
					u32 vertexWeight = bone->mWeights[j].mWeight;

					for (u32 k = 0; k < 4; ++k)
					{
						JGBoneVertex& boneVertex = boneVertices[vertexID];
						if (boneVertex.BoneWeights[k] == 0.0f)
						{
							boneVertex.BoneIDs[k]	  = vertexID;
							boneVertex.BoneWeights[k] = vertexWeight;
							break;
						}
					}
				}
			}
			output->BoneVertices.push_back(boneVertices);
		}


	}
	void AssetImporter::ReadSkeletal(const aiScene* scene, const aiMesh* mesh, SkeletalAssetStock* output)
	{
		Dictionary<String, u32> boneNodeDIc;

		ReadSkeletalNode(mesh, boneNodeDIc, output);

		output->RootBoneNode = -1;
		ReadSkeletalHierarchy(scene->mRootNode, boneNodeDIc, output);

	}
	void AssetImporter::ReadSkeletalNode(const aiMesh* mesh, Dictionary<String, u32>& boneNodeDIc,  SkeletalAssetStock* output)
	{
		u32 boneCount = mesh->mNumBones;

		std::function<JMatrix(const aiMatrix4x4&)> toJMatrix = [](const aiMatrix4x4& aiMatrix) -> JMatrix
		{
			return JMatrix(
				aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4,
				aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4,
				aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4,
				aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4);
		};

		output->BoneNodes.resize(boneCount);

		for (u32 i = 0; i < boneCount; ++i)
		{
			aiBone* bone = mesh->mBones[i];


			SkeletalAssetStock::BoneNode& boneNode = output->BoneNodes[i];
			boneNode.ID = i;
			boneNode.Name = bone->mName.C_Str();
			boneNode.BoneOffset = toJMatrix(bone->mOffsetMatrix);
			boneNodeDIc[boneNode.Name] = i;
		}



	}
	void AssetImporter::ReadSkeletalHierarchy(const aiNode* node, const Dictionary<String, u32>& boneNodeDic, SkeletalAssetStock* output)
	{
		std::function<JMatrix(const aiMatrix4x4&)> toJMatrix = [](const aiMatrix4x4& aiMatrix) -> JMatrix
		{
			return JMatrix(
				aiMatrix.a1, aiMatrix.a2, aiMatrix.a3, aiMatrix.a4,
				aiMatrix.b1, aiMatrix.b2, aiMatrix.b3, aiMatrix.b4,
				aiMatrix.c1, aiMatrix.c2, aiMatrix.c3, aiMatrix.c4,
				aiMatrix.d1, aiMatrix.d2, aiMatrix.d3, aiMatrix.d4);
		};
		if (boneNodeDic.find(node->mName.C_Str()) != boneNodeDic.end())
		{
			u32 boneID = boneNodeDic.at(node->mName.C_Str());
			SkeletalAssetStock::BoneNode& boneNode = output->BoneNodes[boneID];

			if (output->RootBoneNode == -1)
			{
				output->RootBoneNode = boneID;
				boneNode.ParentNode = -1;
			}
			else
			{
				aiNode* parentNode	= node->mParent;
				u32 parentBoneID	= boneNodeDic.at(parentNode->mName.C_Str());
				SkeletalAssetStock::BoneNode& parentBoneNode = output->BoneNodes[parentBoneID];

				boneNode.ParentNode = parentBoneID;
				parentBoneNode.ChildNodes.push_back(boneID);
			}
			boneNode.Transform = toJMatrix(node->mTransformation);
		}

		for (u32 i = 0; i < node->mNumChildren; ++i)
		{
			ReadSkeletalHierarchy(node->mChildren[i], boneNodeDic, output);
		}
	}
	void AssetImporter::ReadAnimation(const aiAnimation* anim, AnimationClipAssetStock* output)
	{
		if (output == nullptr || anim == nullptr)
		{
			return;
		}
		output->Name = anim->mName.C_Str();

		// Duration
		output->Duration = anim->mDuration;

		// tickperSecond
		output->TicksPerSecond = anim->mTicksPerSecond;

	
		u32 channelCnt = anim->mNumChannels;
		for (u32 i = 0; i < channelCnt; ++i)
		{
			aiNodeAnim* channel = anim->mChannels[i];

			AnimationClipAssetStock::AnimationNode animNode;
			animNode.NodeName = channel->mNodeName.C_Str();
			
			for(u32 k = 0; k < channel->mNumPositionKeys; ++k)
			{
				const aiVectorKey& key = channel->mPositionKeys[k];
				animNode.LocationTimes.push_back(key.mTime);
				animNode.LocationValues.push_back(JVector3(key.mValue.x, key.mValue.y, key.mValue.z));
			}
			for (u32 k = 0; k < channel->mNumRotationKeys; ++k)
			{
				const aiQuatKey& key = channel->mRotationKeys[k];
				animNode.RotationTimes.push_back(key.mTime);
				animNode.RotationValues.push_back(JQuaternion(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w));
			}
			for (u32 k = 0; k < channel->mNumScalingKeys; ++k)
			{
				const aiVectorKey& key = channel->mScalingKeys[k];
				animNode.ScaleTimes.push_back(key.mTime);
				animNode.ScaleValues.push_back(JVector3(key.mValue.x, key.mValue.y, key.mValue.z));
			}

			output->AnimationNodes[animNode.NodeName] =  animNode;
		}
	}
	void AssetImporter::ReadTexture(const aiTexture* tex, TextureAssetStock* output)
	{
		if (output == nullptr || tex == nullptr)
		{
			return;
		}

		output->Name     = tex->mFilename.C_Str();
		output->Width    = tex->mWidth;
		output->Height   = tex->mHeight;
		output->Channels = 4;

		u32 pixelSize = output->Width * output->Height * output->Channels;
		output->OriginPixelSize = pixelSize;

		output->Pixels.resize(pixelSize);
		i32 result = compress((Bytef*)(output->Pixels.data()), (uLongf*)(&pixelSize), (const Bytef*)(tex->pcData), pixelSize);
		output->Pixels.resize(pixelSize);
		if (result != Z_OK)
		{
			
		}
	}
	void AssetImporter::ReadMaterial(const aiMaterial* mat, MaterialAssetStock* output)
	{
		if (output == nullptr || mat == nullptr)
		{
			return;
		}
		output->Name = mat->GetName().C_Str();

		u32 propertyCnt = mat->mNumProperties;
		for (u32 i = 0; i < propertyCnt; ++i)
		{
			aiMaterialProperty* property = mat->mProperties[i];


			u32 textureIndex = property->mIndex;
			String propertyName = property->mKey.C_Str();
			aiPropertyTypeInfo typeInfo = property->mType;


		}


	}
	void AssetImporter::WriteMesh(const String& outputPath, StaticMeshAssetStock& stock)
	{
		if (stock.Name.empty() == true)
		{
			stock.Name = stock.SubMeshNames[0];
		}

		auto filePath = PathHelper::CombinePath(outputPath, stock.Name) + JG_ASSET_FORMAT;
		auto json = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_KEY, stock);

		if (AssetDataBase::GetInstance().WriteAsset(filePath, EAssetFormat::Mesh, json) == false)
		{
			JG_LOG_ERROR("Fail Write Mesh : {0} ", outputPath);
		}
	}

	void AssetImporter::WriteSkeletal(const String& outputPath, SkeletalAssetStock& stock)
	{
		auto filePath = PathHelper::CombinePath(outputPath, stock.Name) + JG_ASSET_FORMAT;
		auto json = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_KEY, stock);

		if (AssetDataBase::GetInstance().WriteAsset(filePath, EAssetFormat::Skeletal, json) == false)
		{
			JG_LOG_ERROR("Fail Write Mesh : {0} ", outputPath);
		}
	}

	void AssetImporter::WriteAnimation(const String& outputPath, AnimationClipAssetStock& stock)
	{
		auto filePath = PathHelper::CombinePath(outputPath, stock.Name) + JG_ASSET_FORMAT;

		auto json = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_KEY, stock);
		if (AssetDataBase::GetInstance().WriteAsset(filePath, EAssetFormat::AnimationClip, json) == false)
		{
			JG_LOG_ERROR("Fail Write AnimationClip : {0} ", outputPath);
		}
	}

	void AssetImporter::WriteTexture(const String& outputPath, TextureAssetStock& stock)
	{
		auto filePath = PathHelper::CombinePath(outputPath, stock.Name) + JG_ASSET_FORMAT;

		auto json = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_KEY, stock);
		if (AssetDataBase::GetInstance().WriteAsset(filePath, EAssetFormat::Texture, json) == false)
		{
			JG_LOG_ERROR("Fail Write Texture : {0} ", outputPath);
		}
	}

	void AssetImporter::WriteCubeMap(const String& outputPath, TextureAssetStock& stock)
	{
		auto filePath = PathHelper::CombinePath(outputPath, stock.Name) + JG_ASSET_FORMAT;


		auto json = CreateSharedPtr<Json>();
		json->AddMember(JG_ASSET_KEY, stock);
		if (AssetDataBase::GetInstance().WriteAsset(filePath, EAssetFormat::CubeMap, json) == false)
		{
			JG_LOG_ERROR("Fail Write CubeMap : {0} ", outputPath);
		}
	}

	void AssetImporter::WriteMaterial(const String& outputPath, MaterialAssetStock& stock)
	{
		if (MaterialAssetStock::Write(outputPath, stock) == false)
		{
			JG_LOG_ERROR("Fail Write Material : {0} ", outputPath);
		}
	}

}