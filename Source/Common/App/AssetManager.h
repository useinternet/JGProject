#pragma once


#define MESH_FILEFORMAT ".mesh"



enum FBX_ImportFlags
{
	FBX_ImportFlag_SkeletalMesh    = 0x00001,
	FBX_ImportFlag_ImportMesh      = 0x00002,
	FBX_ImportFlag_ImportAnimation = 0x00004,
};

enum AssetError
{
	AssetError_FailReadFile,
};


class AssetManager
{
public:
	bool Import(const std::wstring& path, const std::wstring& output_path,  FBX_ImportFlags flag, std::wstring* errorCode);
private:
	

};