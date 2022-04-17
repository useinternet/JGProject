#pragma once
#include "Common/Define.h"
#include "Math/JVector.h"
#include "Math/JMatrix.h"
#include "Class/FileIO.h"
#include "Asset.h"


struct aiMesh;
struct aiBone;
struct aiNode;
struct aiAnimation;
struct aiTexture;
struct aiMaterial;
struct aiScene;

namespace JG
{
	class IShader;
	class IShaderScript;
	enum class EAssetImportResult
	{
		Success,
		Fail,
	};

	ENUM_FLAG(EFBXAssetImportFlags)
	enum class EFBXAssetImportFlags
	{
		None,
		Import_Mesh			 = 0x001,
		Import_AnimationClip = 0x002,
		Import_Skeletal		 = 0x004,
		Import_Material	     = 0x008,
		Import_Texture       = 0x010,
		Import_All = Import_Mesh | Import_AnimationClip | Import_Skeletal | Import_Material | Import_Texture,
	};

	enum class ETextureAssetImportFlags
	{
		None,
	};
	enum class ECubeMapAssetImportFlags
	{
		None,
	};
	struct FBXAssetImportSettings
	{
		String AssetPath;
		String OutputPath;
		EFBXAssetImportFlags Flags = EFBXAssetImportFlags::None;
	};
	struct TextureAssetImportSettings
	{
		String AssetPath;
		String OutputPath;
		ETextureAssetImportFlags Flags = ETextureAssetImportFlags::None;
	};
	struct CubeMapAssetImportSettings
	{
		String Name;
		// +X, -X, +Y, -Y, +Z, -Z
		String AssetPath[6];
		String OutputPath;
		ECubeMapAssetImportFlags Flags = ECubeMapAssetImportFlags::None;
	};
	struct MaterialAssetImportSettings
	{
		String Shader;
		String ShaderScript;
		String FileName;
		String OutputPath;
	};

	class AssetImporter
	{
	public:
		static EAssetImportResult Import(const FBXAssetImportSettings& setting);
		static EAssetImportResult Import(const TextureAssetImportSettings& settings);
		static EAssetImportResult Import(const MaterialAssetImportSettings& settings); 
		static EAssetImportResult Import(const CubeMapAssetImportSettings& settings);
	private:
		static void ReadMesh(const aiMesh* mesh, StaticMeshAssetStock* output);
		static void ReadSkeletal(const aiScene* scene, const aiMesh* mesh, SkeletalAssetStock* output);
		static void ReadSkeletalNode(const aiMesh* mesh, Dictionary<String, u32>& boneNodeDic, SkeletalAssetStock* output);
		static void ReadSkeletalHierarchy(const aiNode* node, const Dictionary<String, u32>& boneNodeDic, SkeletalAssetStock* output);
		static void ReadAnimation(const aiAnimation* anim, AnimationClipAssetStock* output);
		static void ReadTexture(const aiTexture* tex, TextureAssetStock* output);
		static void ReadMaterial(const aiMaterial* mat, MaterialAssetStock* output);
	private:
		static void WriteMesh(const String& outputPath, StaticMeshAssetStock& stock);
		static void WriteSkeletal(const String& outputPath, SkeletalAssetStock& stock);
		static void WriteAnimation(const String& outputPath, AnimationClipAssetStock& stock);
		static void WriteTexture(const String& outputPath, TextureAssetStock& stock);
		static void WriteCubeMap(const String& outputPath, TextureAssetStock& stock);
		static void WriteMaterial(const String& outputPath, MaterialAssetStock& stock);
	};
}