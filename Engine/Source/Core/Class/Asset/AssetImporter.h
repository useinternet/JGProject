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
		static void ReadMesh(aiMesh* mesh, StaticMeshAssetStock* output);
	private:
		static void WriteMesh(const String& outputPath, StaticMeshAssetStock& info);
		static void WriteTexture(const String& outputPath, TextureAssetStock& stock);
		static void WriteCubeMap(const String& outputPath, TextureAssetStock& stock);
		static void WriteMaterial(const String& outputPath, MaterialAssetStock& stock);
	};
}