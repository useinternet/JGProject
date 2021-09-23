#pragma once
#include "Common/Define.h"

#include "Math/JVector.h"
#include "Math/JMatrix.h"
#include "Class/FileIO.h"
#include "Asset.h"


struct aiMesh;

namespace JG
{
	class IShader;
	class IShaderScript;
	enum class EAssetImportResult
	{
		Success,
		Fail,
		
	};

	enum class EFBXAssetImportFlags
	{
		None,
	};
	enum class ETextureAssetImportFlags
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
	private:
		static void ReadMesh(aiMesh* mesh, StaticMeshAssetStock* output);
	private:
		static void WriteMesh(const String& outputPath, StaticMeshAssetStock& info);
		static void WriteTexture(const String& outputPath, TextureAssetStock& stock);
		static void WriteMaterial(const String& outputPath, MaterialAssetStock& stock);
	};
}