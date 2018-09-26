#include "StaticMesh2DComponent.h"
#include"../../RenderSystem/JGTexture.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShader.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShaderData.h"
#include"../../RenderSystem/JGMaterial/JG2DMesh.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGBuffer.h"
#include"../../RenderSystem/JGDeviceD.h"
#include"../../RenderSystem/JGViewportD.h"
#include"../../EngineStatics/JGConstructHelper.h"
#include"../../RenderSystem/ShaderCode/HLSLConstantBufferDesc.h"

using namespace std;
StaticMesh2DComponent::StaticMesh2DComponent()
{
	RegisterComponentID(typeid(this));
	m_MatrixBufferDesc  = make_unique<SMatrixBuffer_VS>();
	m_2DSpriteBufferDesc = make_unique<S2DSpriteBuffer_PS>();
}
StaticMesh2DComponent::~StaticMesh2DComponent()
{
}

void StaticMesh2DComponent::SetConstructObject(ConstructObject* Object)
{
	StaticMesh2DObject* object = dynamic_cast<StaticMesh2DObject*>(Object);
	if (object)
	{
		m_ShaderName = object->ShaderName;
		m_Texture    = object->Texture.get();
		m_Mesh       = object->Mesh.get();
		m_pPivot     = object->Pivot.get();
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

void StaticMesh2DComponent::SpriteSwichOn(const float r, const float g, const float b)
{
	m_2DSpriteBufferDesc->IsSprite = 1.0f;
	m_2DSpriteBufferDesc->SprietColor = { r,g,b };
}
void StaticMesh2DComponent::SpriteSwichOn(JGVector3D& color)
{
	m_2DSpriteBufferDesc->IsSprite = 1.0f;
	m_2DSpriteBufferDesc->SprietColor = { color.Get() };
}
void StaticMesh2DComponent::SetColorRize(const float r, const float g, const float b)
{
	m_2DSpriteBufferDesc->ColorRize.x = r;
	m_2DSpriteBufferDesc->ColorRize.y = g;
	m_2DSpriteBufferDesc->ColorRize.z = b;
}
void StaticMesh2DComponent::SetColorRize(JGVector3D& color)
{
	m_2DSpriteBufferDesc->ColorRize.x = color.X();
	m_2DSpriteBufferDesc->ColorRize.y = color.Y();
	m_2DSpriteBufferDesc->ColorRize.z = color.Z();
}
void StaticMesh2DComponent::SetAlphaBlend(const float a)
{
	m_2DSpriteBufferDesc->ColorRize.w = a;
}
JGVector2D StaticMesh2DComponent::GetCenterPoint()
{
	float HalfWidth = GetTextureWdith() / 2;
	float HalfHeight = GetTextureHeight() / 2;
	JGVector2D vec;
	switch (GetPivot())
	{
	case EPivot::TopLeft:
		vec.Set(HalfWidth, HalfHeight);
		break;
	case EPivot::TopMiddle:
		vec.Set(0.0f, HalfHeight);
		break;
	case EPivot::TopRight:
		vec.Set(-HalfWidth, HalfHeight);
		break;
	case EPivot::MiddleLeft:
		vec.Set(HalfWidth, 0.0f);
		break;
	case EPivot::MiddleMiddle:
		vec.Set(0.0f, 0.0f);
		break;
	case EPivot::MiddleRight:
		vec.Set(-HalfWidth, 0.0f);
		break;
	case EPivot::BottomLeft:
		vec.Set(HalfWidth, -HalfHeight);
		break;
	case EPivot::BottomMiddle:
		vec.Set(0.0f, -HalfHeight);
		break;
	case EPivot::BottomRight:
		vec.Set(-HalfWidth, -HalfHeight);
		break;

	}
	return GetComponentWorldLocation() + vec;
}
void StaticMesh2DComponent::Render()
{
	Component::Render();
	if (Render2DCurling())
	{
		return;
	}
	JGMatrix worldMatrix = GetWorldMatrix();
	JGMatrix orthoMatrix = GetOrthoMatrix();
	JGMatrix viewMatrix = GetViewMatrix();

	worldMatrix.Transpose();
	viewMatrix.Transpose();
	orthoMatrix.Transpose();


	m_MatrixBufferDesc->WoldViewProjectionMatrix = (orthoMatrix * viewMatrix * worldMatrix).Get();


	JGShaderData Data;
	Data.InsertData(TT("MatrixBuffer"), m_MatrixBufferDesc.get());
	ShaderParamSetting(&Data);
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

void StaticMesh2DComponent::ShaderParamSetting(JGShaderData* Data)
{
	Data->InsertData(TT("2DSpriteBuffer"), m_2DSpriteBufferDesc.get());
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


EPivot StaticMesh2DComponent::GetPivot()
{
	return *m_pPivot;
}

void StaticMesh2DComponent::SetPivot(EPivot* pivot)
{
	m_pPivot = pivot;
}
bool StaticMesh2DComponent::Render2DCurling()
{
	if (!m_Mesh)
	{
		return true;
	}
	float ScreenWidth = GetViewport()->GetWidth();
	float ScreenHeight = GetViewport()->GetHeight();

	float LocationX = GetComponentWorldLocation().X();
	float LocationY = GetComponentWorldLocation().Y();
	float MeshWidth = m_Mesh->GetMeshWidth();
	float MeshHeight = m_Mesh->GetMeshHeight();
	EPivot MeshPivot = m_Mesh->GetPivot();

	switch (MeshPivot)
	{
	case EPivot::TopLeft:
		break;
	case EPivot::TopMiddle:
		LocationX -= (MeshWidth / 2);
		break;
	case EPivot::TopRight:
		LocationX -= MeshWidth;
		break;
	case EPivot::MiddleLeft:
		LocationY -= (MeshHeight / 2);
		break;
	case EPivot::MiddleMiddle:
		LocationX -= (MeshWidth / 2);
		LocationY -= (MeshHeight / 2);
		break;
	case EPivot::MiddleRight:
		LocationY -= (MeshHeight / 2);
		LocationX -= MeshWidth;
		break;
	case EPivot::BottomLeft:
		LocationY += MeshHeight;
		break;
	case EPivot::BottomMiddle:
		LocationY += MeshHeight;
		LocationX -= (MeshWidth / 2);
		break;
	case EPivot::BottomRight:
		LocationY += MeshHeight;
		LocationX -= MeshWidth;
		break;
	}

	if (LocationX + MeshWidth < 0 || LocationY + MeshHeight < 0)
	{
		return true;
	}
	else if (LocationX > ScreenWidth || LocationY > ScreenHeight)
	{
		return true;
	}
	return false;
}
