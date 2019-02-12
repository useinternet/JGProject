#include"Object.h"
#include"Data/Mesh/JGBaseMesh.h"
using namespace JGRC;
using namespace std;

Object::Object(EObjectType type, const string& name)
{
	m_Type = type;
	m_Name = name;
}
void Object::SetMesh(JGBaseMesh* mesh, const string& meshName)
{
	m_Mesh = mesh;
	m_MeshName = meshName;
}
JGBaseMesh* Object::GetMesh()
{
	return m_Mesh;
}