#include "StaticMesh2DComponent.h"
#include"../../RenderSystem/JGTexture.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShader.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShaderData.h"
#include"../../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGBuffer.h"
#include"../../RenderSystem/JGDeviceD.h"
#include"../../RenderSystem/ShaderCode/SampleShaderDesc.h"
#include"../../RenderSystem/JGViewportD.h"
#include"../../EngineStatics/JGConstructHelper.h"
using namespace std;
StaticMesh2DComponent::StaticMesh2DComponent()
{
	RegisterComponentID(typeid(this));
}
StaticMesh2DComponent::~StaticMesh2DComponent()
{
}

bool StaticMesh2DComponent::ConstructMesh2D(const std::wstring & TexturePath)
{
	
	static JGConstructHelper::StaticMesh2D TempMesh2DComponent(
		GetDevice(), GetBufferManager(), GetComponentName(),EPivot::MiddleMiddle,
		TexturePath);
	if (TempMesh2DComponent.Success)
	{
		SetStaticMesh2DComponent(TempMesh2DComponent.Object);
	}

	return true;
}
void StaticMesh2DComponent::SetStaticMesh2DComponent(StaticMesh2DObject* Object)
{
	m_ShaderName = Object->ShaderName;
	m_Texture    = Object->Texture.get();
	m_Mesh       = Object->Mesh.get();
}
float StaticMesh2DComponent::GetTextureWdith()
{
	if (m_Texture)
	{
		return (float)m_Texture->GetInformation(0).Width;
	}
	return 0.0f;
}
float StaticMesh2DComponent::GetTextureHeight()
{
	if (m_Texture)
	{
		return (float)m_Texture->GetInformation(0).Height;
	}
	return 0.0f;
}
JG2DMesh* StaticMesh2DComponent::GetMesh()
{
	return m_Mesh;
}
JGTexture* StaticMesh2DComponent::GetTexture()
{
	return m_Texture;
}
void StaticMesh2DComponent::Render()
{
	Component::Render();

	JGMatrix worldMatrix = GetWorldMatrix();
	JGMatrix projectionMatrix = GetOrthoMatrix();
	JGMatrix viewMatrix = GetViewMatrix();

	worldMatrix.Transpose();
	viewMatrix.Transpose();
	projectionMatrix.Transpose();


	//¼ÎÀÌ´õ¿ë µ¥ÀÌÅÍ »ý¼º
	JGShaderData Data;
	SMatrixBuffer desc;
	desc.worldMatrix = worldMatrix.Get();
	desc.projectionMatirx = projectionMatrix.Get();
	desc.viewMatrix = viewMatrix.Get();
	// ¼¼ÀÌ´õ¿ë µ¥ÀÌÅÍ »ðÀÔ
	Data.InsertData(TT("MatrixBuffer"), (void*)&desc);
	//
	// ¸Þ½¬ ·»´õ¸µ
	if (m_Mesh)
	{
		m_Mesh->Render(GetDevice(), ERenderingType::TriangleList);
	}
	// ¼ÎÀÌ´õ ·»´õ¸µ
	GetHLSLDevice()->Render(m_ShaderName, &Data, m_Texture, 6);
}