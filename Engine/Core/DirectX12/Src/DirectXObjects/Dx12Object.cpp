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
	m_Name(name), m_InstanceCount(0), m_ShaderPath(MAIN_SHADER_PATH)
{
	m_BuildOK = false;
}
void Dx12Object::SetMesh(const Mesh& mesh)
{
	m_Mesh = mesh;
	m_BuildOK = false;
}
void Dx12Object::SetName(const string& name)
{
	m_Name = name;
}


ObjectFragments* Dx12Object::AddObjectFragments()
{
	// ObjectCB 추가 SkinnedCB추가
	m_ObjectCBDatas.push_back(ObjectCB());

	// 인스턴스 오브젝트 만들기
	auto Sub_Obj = make_unique<ObjectFragments>(this, m_InstanceCount);
	

	ObjectFragments* result = Sub_Obj.get();
	m_ObjectFragments.push_back(move(Sub_Obj));

	
	// 인스턴스 카운터 증가
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
	// 임시 CB 변수 생성
	std::shared_ptr<ObjectFragments> tempObj = m_ObjectFragments[m_InstanceCount - 1];
	auto tempObjectCB = m_ObjectCBDatas[m_InstanceCount - 1];

	// 해당 인스턴스 오브젝트가 이 오브젝트 소속이 아닐경우 return
	if (m_InstanceCount < obj->GetInstanceID() ||
		m_ObjectFragments[obj->GetInstanceID()].get() != obj)
	{
		DX12_LOG_WARNING("ObjectFragments is not exsit in %s object", m_Name.c_str());
		return;
	}
	// 마지막 인스턴스 오브젝트엿다면 pop_back;
	if (tempObj.get() == obj)
	{
		m_ObjectFragments.pop_back();
		m_ObjectCBDatas.pop_back();

		
		m_InstanceCount--;
	}
	// 그게 아니라면 뒤에있는원소와 맞바꾸어 pop_back;
	else
	{
		int id = obj->GetInstanceID();


		m_ObjectFragments[m_InstanceCount - 1] = m_ObjectFragments[id];
		m_ObjectFragments[id] = tempObj;
		m_ObjectFragments.pop_back();


		m_ObjectCBDatas[m_InstanceCount - 1] = m_ObjectCBDatas[id];
		m_ObjectCBDatas[id] = tempObjectCB;
		m_ObjectCBDatas.pop_back();
		

		// id 갱신S
		m_ObjectFragments[id]->m_InstanceID = id;
		m_ObjectFragments[id]->m_Name = m_Name + to_string(id);
		m_InstanceCount--;
	}
}
void Dx12Object::SetSkinnedCB(const SkinnedCB& cb)
{
	m_SkinnedCB = cb;
}
void Dx12Object::Build(const std::string& shaderpath,
	const std::vector<EShaderStage>& shaderstage,
	EPreparedPSO pso_version)
{
	// mesh가 존재하지않다면 build 실패
	if (m_Mesh.GetIndexCount() == 0)
	{
		DX12_LOG_ERROR("%s Object have not Mesh", m_Name.c_str());
		m_BuildOK = false;
		return;
	}
	m_ShaderPath = shaderpath;
	m_ShaderStage = shaderstage;
	m_PreparedPSO = pso_version;

	Build();
}

void Dx12Object::Update()
{
	if (!m_BuildOK)
	{
		Build();
	}

	for (uint32_t i = 0; i < m_InstanceCount; ++i)
	{
		m_ObjectCBDatas[i].World = m_ObjectFragments[i]->GetTransform_c().GetHlslMatrix().Get();
		m_ObjectCBDatas[i].MaterialIndex = m_ObjectFragments[i]->GetMaterial()->GetID();
	}
}
void Dx12Object::Draw(CommandList* commandList) const
{
	if (!m_BuildOK)
		return;
	commandList->SetPipelineState(m_PSO);


	commandList->SetGraphicsShaderResourceView(CommonRootParam::ObjectCB, m_ObjectCBDatas);
	commandList->SetGraphicsConstantBufferView(CommonRootParam::SkinnedCB, &m_SkinnedCB);
	if (!m_TextureSRVHandles.empty())
	{
		commandList->SetGraphicsDescriptorTables(
			CommonRootParam::Texture,
			(uint32_t)m_TextureSRVHandles.size(),
			m_TextureSRVHandles.data());
	}
	if (!m_CubeTextureSRVHandles.empty())
	{
		commandList->SetGraphicsDescriptorTables(
			CommonRootParam::CubeMap,
			(uint32_t)m_CubeTextureSRVHandles.size(),
			m_CubeTextureSRVHandles.data());
	}
	m_Mesh.Draw(commandList, m_InstanceCount);
}

void Dx12Object::AddTexture(const Texture& texture)
{
	if (m_Textures.size() >= TextureSlot::NumSlot)
	{
		DX12_LOG_WARNING("object's max texture slot exceed");
		return;
	}
	m_Textures.push_back(texture);
	m_TextureSRVHandles.push_back(texture.GetShaderResourceView());
	m_BuildOK = false;
}
void Dx12Object::AddTextures(const std::vector<Texture>& textures)
{
	if (m_Textures.size() + textures.size() > TextureSlot::NumSlot)
	{
		DX12_LOG_WARNING("object's max texture slot exceed");
		return;
	}
	for (auto& texture : textures)
	{
		m_Textures.push_back(texture);
		m_TextureSRVHandles.push_back(texture.GetShaderResourceView());
	}
	m_BuildOK = false;
}
void Dx12Object::PopTexture(uint32_t numTexture)
{
	if (m_Textures.empty())
		return;
	for (uint32_t i = 0; i < numTexture; ++i)
	{
		m_Textures.pop_back();
		m_TextureSRVHandles.pop_back();
	}
	m_BuildOK = false;
}
void Dx12Object::SetTexture(ETextureSlot slot, const Texture& texture)
{
	if (m_Textures.size() <= slot)
	{
		DX12_LOG_ERROR("%s 's Texture is not register slot", m_Name.c_str());
		return;
	}
	m_Textures[slot] = texture;
	m_TextureSRVHandles[slot] = texture.GetShaderResourceView();
}


void Dx12Object::AddCubeTexture(const Texture& texture)
{
	if (m_CubeTextures.size() >= CubeTextureSlot::NumSlot)
	{
		DX12_LOG_WARNING("object's max cubetexture slot exceed");
		return;
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texture.GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = texture.GetDesc().MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	m_CubeTextures.push_back(texture);
	m_CubeTextureSRVHandles.push_back(texture.GetShaderResourceView(&srvDesc));

	m_BuildOK = false;
}
void Dx12Object::AddCubeTextures(const std::vector<Texture>& textures)
{
	if (m_CubeTextures.size() + textures.size() >= CubeTextureSlot::NumSlot)
	{
		DX12_LOG_WARNING("object's max cubetexture slot exceed");
		return;
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	for (auto& texture : textures)
	{
		srvDesc.Format = texture.GetDesc().Format;
		srvDesc.TextureCube.MipLevels = texture.GetDesc().MipLevels;

		m_CubeTextures.push_back(texture);
		m_CubeTextureSRVHandles.push_back(texture.GetShaderResourceView(&srvDesc));
	}
	m_BuildOK = false;
}
void Dx12Object::PopCubeTexture(uint32_t numTexture)
{
	if (m_CubeTextures.empty())
		return;
	for (uint32_t i = 0; i < numTexture; ++i)
	{
		m_CubeTextures.pop_back();
		m_CubeTextureSRVHandles.pop_back();
	}
	m_BuildOK = false;
}
void Dx12Object::SetCubeTexture(ECubeTextureSlot slot, const Texture& texture)
{
	if (m_CubeTextures.size() <= slot)
	{
		DX12_LOG_ERROR("%s 's Texture is not register slot", m_Name.c_str());
		return;
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = texture.GetDesc().Format;
	srvDesc.TextureCube.MipLevels = texture.GetDesc().MipLevels;
	m_CubeTextures[slot] = texture;
	m_CubeTextureSRVHandles[slot] = texture.GetShaderResourceView(&srvDesc);
}


void Dx12Object::Build()
{
	// Shader 컴파일
	GraphicsShader shader;
	shader.AddShaderPaths(m_ShaderStage, m_ShaderPath);
	m_Mesh.RegisterShaderMacro(shader);
	// 텍스쳐 매크로 등록
	for (uint32_t i = 0; i < (uint32_t)m_Textures.size(); ++i)
	{
		shader.AddMacro(ShaderStage::Pixel, SHADER_MACRO_USE_TEXTURE_SLOT(i));
	}
	for (uint32_t i = 0; i < (uint32_t)m_CubeTextures.size(); ++i)
	{
		shader.AddMacro(ShaderStage::Pixel, SHADER_MACRO_USE_CUBETEXTURE_SLOT(i));
	}
	m_BuildOK = shader.Compile();
	if (!m_BuildOK)
	{
		DX12_LOG_ERROR("%s Object is failed build", m_Name.c_str());
	}
	else
	{
		m_PSO = DxDevice::GetShaderCommonDefines()->GetPSO(m_PreparedPSO, shader);
	}
	if (m_InstanceCount == 0)
	{
		AddObjectFragments();
	}
}