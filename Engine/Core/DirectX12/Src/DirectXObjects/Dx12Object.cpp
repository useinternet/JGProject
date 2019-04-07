#include<PCH.h>
#include"Dx12Object.h"
#include"DirectXToolKit/GeometryGenerator.h"
#include"DirectXToolKit/CommandList.h"
#include"DirectXToolKit/DirectXCore.h"
#include"DirectXToolKit/Shader.h"
using namespace std;
using namespace Dx12;
using namespace Common;


Dx12Object::Dx12Object(const std::string& name) : 
	m_Name(name), m_InstanceCount(0)
{
	m_BuildOK = false;
}
void Dx12Object::SetMesh(const Mesh& mesh)
{
	m_Mesh = mesh;
}
void Dx12Object::SetName(const string& name)
{
	m_Name = name;
}


ObjectFragments* Dx12Object::AddObjectFragments()
{
	m_ObjectCBDatas.push_back(ObjectCB());

	auto Sub_Obj = make_unique<ObjectFragments>(this);
	Sub_Obj->m_Name = m_Name + to_string(m_InstanceCount);
	Sub_Obj->m_InstanceID = m_InstanceCount;

	ObjectFragments* result = Sub_Obj.get();
	m_ObjectFragments.push_back(move(Sub_Obj));



	m_InstanceCount++;
	return result;
}
ObjectFragments* Dx12Object::GetObjectFragments(uint32_t instanceID)
{
	if (m_InstanceCount <= instanceID)
	{
		DX12_LOG_ERROR("%s 's instnace ID(%d) is not exit in Dx12Object", m_Name.c_str(), instanceID);
		return nullptr;
	}
	return m_ObjectFragments[instanceID].get();
}
void Dx12Object::RemoveObjectFragments(ObjectFragments* obj)
{
	std::shared_ptr<ObjectFragments> tempObj = m_ObjectFragments[m_InstanceCount - 1];
	auto tempCB = m_ObjectCBDatas[m_InstanceCount - 1];

	if (m_InstanceCount < obj->GetInstanceID() ||
		m_ObjectFragments[obj->GetInstanceID()].get() != obj)
	{
		DX12_LOG_WARNING("ObjectFragments is not exsit in %s object", m_Name.c_str());
		return;
	}

	if (tempObj.get() == obj)
	{
		m_ObjectFragments.pop_back();
		m_ObjectCBDatas.pop_back();
		m_InstanceCount--;
	}
	else
	{
		int id = obj->GetInstanceID();
		m_ObjectFragments[m_InstanceCount - 1] = m_ObjectFragments[id];
		m_ObjectFragments[id] = tempObj;
		m_ObjectCBDatas[m_InstanceCount - 1] = m_ObjectCBDatas[id];
		m_ObjectCBDatas[id] = tempCB;

		m_ObjectFragments.pop_back();
		m_ObjectCBDatas.pop_back();

		m_ObjectFragments[id]->m_InstanceID = id;
		m_ObjectFragments[id]->m_Name = m_Name + to_string(id);
		m_InstanceCount--;
	}
}
void Dx12Object::Build(CommandList* commandList)
{
	if (m_Mesh.GetIndexCount() == 0)
	{
		m_Mesh = GeometryGenerator::CreateSphere(commandList, 5.0f, 20, 20);
	}


	GraphicsShader shader;
	shader.AddShaderPaths({ ShaderStage::Vertex, ShaderStage::Pixel }, MAIN_SHADER_PATH);
	m_Mesh.RegisterShaderMacro(shader);


	m_BuildOK = shader.Compile();
	if (!m_BuildOK)
	{
		DX12_LOG_ERROR("%s Object is failed build", m_Name.c_str());
	}
	else
	{
		m_PSO = DxDevice::GetShaderCommonDefines()->GetMainPSO(shader);
	}
}
void Dx12Object::Update(SkinnedCB* skinnedCB)
{
	if (m_InstanceCount == 0)
	{
		AddObjectFragments();
	}
	if (skinnedCB)
	{
		m_SkinnedCB = *skinnedCB;
	}
	for (uint32_t i = 0; i < m_InstanceCount; ++i)
	{
		m_ObjectCBDatas[i].World = m_ObjectFragments[i]->GetTransform_c().GetHlslMatrix().Get();
	}
}
void Dx12Object::Draw(CommandList* commandList)
{
	if (!m_BuildOK)
		return;
	commandList->SetPipelineState(m_PSO);


	commandList->SetGraphicsShaderResourceView(CommonRootParam::ObjectCB, m_ObjectCBDatas);
	commandList->SetGraphicsConstantBufferView(CommonRootParam::SkinnedCB, &m_SkinnedCB);
	

	m_Mesh.Draw(commandList, m_InstanceCount);
}
void Dx12Object::Draw(CommandList* commandList, const GraphicsPSO& pso)
{
	if (!m_BuildOK)
		return;
	commandList->SetPipelineState(pso);


	commandList->SetGraphicsShaderResourceView(CommonRootParam::ObjectCB, m_ObjectCBDatas);
	commandList->SetGraphicsConstantBufferView(CommonRootParam::SkinnedCB, &m_SkinnedCB);


	m_Mesh.Draw(commandList, m_InstanceCount);
}