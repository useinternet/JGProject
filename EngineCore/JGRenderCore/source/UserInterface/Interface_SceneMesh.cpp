#include"Interface_SceneMesh.h"
#include"Data/Mesh/JGBaseMesh.h"
#include"Data/Mesh/JGSkeletalMesh.h"
#include"Data/Mesh/JGStaticMesh.h"
using namespace JGRC;
using namespace std;
Interface_SceneMesh::Interface_SceneMesh(JGBaseMesh* mesh) : m_Data(mesh) {}



Interface_SceneStaticMesh::Interface_SceneStaticMesh(JGBaseMesh* mesh) : Interface_SceneMesh(mesh)
{
	m_StaticData = dynamic_cast<JGStaticMesh*>(mesh);
}
void Interface_SceneStaticMesh::AddStaticMeshArg(const string& path)
{
	m_StaticData->AddStaticMeshArg(path);
}

void Interface_SceneStaticMesh::AddBoxArg(
	const string& name,
	float width,
	float height,
	float depth,
	uint32_t numSubdivision)
{
	m_StaticData->AddBoxArg(name, width, height, depth, numSubdivision);
}

void Interface_SceneStaticMesh::AddSphereArg(
	const string& name,
	float radius,
	UINT32 numSubdivision)
{
	m_StaticData->AddSphereArg(name, radius, numSubdivision);
}

void Interface_SceneStaticMesh::AddSphereArg(
	const string& name,
	float radius,
	UINT32 slicCount,
	UINT32 stackCount)
{
	m_StaticData->AddSphereArg(name, radius, slicCount, stackCount);
}

void Interface_SceneStaticMesh::AddCylinderArg(
	const string& name,
	float bottomRadius,
	float topRadius,
	float height,
	UINT32 sliceCount,
	UINT32 stackCount)
{
	m_StaticData->AddCylinderArg(name, bottomRadius, topRadius, height, sliceCount, stackCount);
}

void Interface_SceneStaticMesh::AddGridArg(
	const string& name,
	float width,
	float depth,
	uint32_t m,
	uint32_t n)
{
	m_StaticData->AddGridArg(name, width, depth, m, n);
}

void Interface_SceneStaticMesh::AddQuadArg(
	const string& name,
	float  x,
	float y,
	float w,
	float h,
	float depth)
{
	m_StaticData->AddQuadArg(name, x, y, w, h, depth);
}
Interface_SceneSkeletalMesh::Interface_SceneSkeletalMesh(JGBaseMesh* mesh) : Interface_SceneMesh(mesh) 
{
	m_SkeletalData = dynamic_cast<JGSkeletalMesh*>(mesh);
}
void Interface_SceneSkeletalMesh::AddSkeletalMeshArg(const string& name)
{
	m_SkeletalData->AddSkeletalMeshArg(name);
}


