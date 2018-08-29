#include "StaticMesh2DComponent.h"
#include"../../RenderSystem/JGTexture.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShader.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShaderData.h"
#include"../../RenderSystem/JGMaterial/JGMesh.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGBuffer.h"
#include"../../RenderSystem/JGDeviceD.h"
#include"../../RenderSystem/ShaderCode/SampleShaderDesc.h"
#include"../../RenderSystem/JGViewportD.h"
using namespace std;
StaticMesh2DComponent::StaticMesh2DComponent()
{
	RegisterComponentID(typeid(this));

	m_Texture = make_unique<JGTexture>();
	m_Mesh = make_unique<JGMesh>();

}

StaticMesh2DComponent::~StaticMesh2DComponent()
{
}

bool StaticMesh2DComponent::ConstructMesh2D(const std::wstring & TexturePath)
{
	// 텍스쳐 추가
	bool result = m_Texture->Add(GetDevice(), TexturePath);
	if (!result)
	{
		return false;
	}
	Create2DMesh();
	// 셰이더 만들고
	m_ShaderName = TT("SampleShader");
	return true;
}
float StaticMesh2DComponent::GetTextureWdith()
{
	return (float)m_Texture->GetInformation(0).Width;
}
float StaticMesh2DComponent::GetTextureHeight()
{
	return (float)m_Texture->GetInformation(0).Height;

}
void StaticMesh2DComponent::Render()
{

	Motivated2DComponent::Render();

	// 임시
	JGMatrix worldMatrix = GetWorldMatrix();
	JGMatrix projectionMatrix = GetOrthoMatrix();
	JGMatrix viewMatrix = GetViewMatrix();
	// JGShaderData 생성 ( 한번 만들어두고 게속 쓸수있도록 만든다.)

	worldMatrix.Transpose();
	viewMatrix.Transpose();
	projectionMatrix.Transpose();


	JGShaderData Data;
	SMatrixBuffer desc;
	desc.worldMatrix = worldMatrix.Get();
	desc.projectionMatirx = projectionMatrix.Get();
	desc.viewMatrix = viewMatrix.Get();

	Data.InsertData(TT("MatrixBuffer"), (void*)&desc);
	//
	// 메쉬 렌더링
	m_Mesh->Render(GetDevice(), ERenderingType::TriangleList);

	// 셰이더 렌더링
	GetHLSLDevice()->Render(m_ShaderName, &Data, m_Texture.get(), 6);
}

void StaticMesh2DComponent::Create2DMesh()
{
	Mesh2DVertexType* vertices;

	vertices = new Mesh2DVertexType[6];
	if (vertices == nullptr)
	{
		return;
	}
	float x = GetTextureWdith() / 2;
	float y = GetTextureHeight() / 2;
	// 정점 생성 .. 일단..
	vertices[0].position = D3DXVECTOR3(-x, y, 0.0f);
	vertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].position = D3DXVECTOR3(x, -y, 0.0f);
	vertices[1].tex = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].position = D3DXVECTOR3(-x, -y, 0.0f);
	vertices[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	vertices[3].position = D3DXVECTOR3(-x, y, 0.0f);
	vertices[3].tex = D3DXVECTOR2(0.0f, 0.0f);
	vertices[4].position = D3DXVECTOR3(x, y, 0.0f);
	vertices[4].tex = D3DXVECTOR2(1.0f, 0.0f);
	vertices[5].position = D3DXVECTOR3(x, -y, 0.0f);
	vertices[5].tex = D3DXVECTOR2(1.0f, 1.0f);
	// 메쉬 생성
	m_Mesh->Create_Vertex_Index_Buffer(
		GetBufferManager(),
		GetComponentName(), 
		EJGUsageType::Static, EJGCPUType::None,
		EJGUsageType::Static, EJGCPUType::None,
		vertices, 6, sizeof(Mesh2DVertexType));

	delete vertices;
	vertices = nullptr;


}