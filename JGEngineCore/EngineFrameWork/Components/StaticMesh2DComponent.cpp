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

void StaticMesh2DComponent::SetConstructObject(ConsructObject* Object)
{
	StaticMesh2DObject* object = dynamic_cast<StaticMesh2DObject*>(Object);
	if (object)
	{
		m_ShaderName = object->ShaderName;
		m_Texture = object->Texture.get();
		m_Mesh = object->Mesh.get();
	}
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
		// ¼ÎÀÌ´õ ·»´õ¸µ
		if (m_ShaderName != TT("None"))
		{
			GetHLSLDevice()->Render(m_ShaderName, &Data, m_Texture, m_Mesh->GetIndexCount());
		}
	}

}

JG2DMesh* StaticMesh2DComponent::GetMesh()
{
	return m_Mesh;
}
void StaticMesh2DComponent::SetMesh(JG2DMesh* mesh)
{
	m_Mesh = mesh;
}
JGTexture* StaticMesh2DComponent::GetTexture()
{
	return m_Texture;
}
void StaticMesh2DComponent::SetTexture(JGTexture* texture)
{
	m_Texture = texture;
}
std::wstring& StaticMesh2DComponent::GetShaderName()
{
	return m_ShaderName;
}
void StaticMesh2DComponent::SetShaderName(const wstring & ShaderName)
{
	m_ShaderName = ShaderName;
}