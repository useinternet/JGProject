#include"MaterialSystem.h"
using namespace std;
using namespace JGRC;
MaterialSystem* MaterialSystem::Instance = nullptr;

MaterialSystem* MaterialSystem::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new MaterialSystem;
	}
	return Instance;
}

void MaterialSystem::Release()
{
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}
Material* MaterialSystem::CreateMaterial()
{
	unique_ptr<Material> m = make_unique<Material>();
	Material* result = m.get();

	m_vMaterials.push_back(move(m));

	return result;
}
