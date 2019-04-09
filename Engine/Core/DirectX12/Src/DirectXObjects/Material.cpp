#include<PCH.h>
#include"Material.h"
#include"DirectXToolKit/Shader.h"
using namespace Dx12;
using namespace std;



vector<MaterialCB>       Material::ms_MaterialCBArray;
map<uint32_t, Material*> Material::ms_MaterialMemorys;
uint32_t Material::ms_MaterialID_Offset = 0;
std::mutex Material::ms_MaterialMutex;



const std::vector<MaterialCB>& Material::GetMaterialCBArray() {
	return ms_MaterialCBArray;
}


Material::Material()
{
	lock_guard<mutex> lock(ms_MaterialMutex);


	ms_MaterialCBArray.push_back(MaterialCB());
	m_ID = ms_MaterialID_Offset++;

	ms_MaterialMemorys[m_ID] = this;
}
Material::~Material()
{
	lock_guard<mutex> lock(ms_MaterialMutex);

	uint32_t id = m_ID;
	uint32_t dest_id = ms_MaterialID_Offset - 1;



	auto tempCB = ms_MaterialCBArray[dest_id];
	ms_MaterialCBArray[dest_id] = ms_MaterialCBArray[id];
	ms_MaterialCBArray[id] = tempCB;


	Material* pMaterial = ms_MaterialMemorys[dest_id];
	pMaterial->m_ID = id;


	ms_MaterialMemorys.erase(dest_id);
	ms_MaterialMemorys[id] = pMaterial;


	--ms_MaterialID_Offset;
}

uint32_t Material::GetID()
{
	return m_ID;
}
MaterialCB& Material::Get()
{
	return ms_MaterialCBArray[m_ID];
}
const MaterialCB& Material::Get_c() const
{
	return ms_MaterialCBArray[m_ID];
}
