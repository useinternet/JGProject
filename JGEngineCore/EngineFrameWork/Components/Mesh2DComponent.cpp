#include "Mesh2DComponent.h"
#include"../../RenderSystem/JGTexture.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShader.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGHLSLShaderDevice.h"
#include"../../RenderSystem/JGHLSLShaderDevice/JGShaderData.h"
#include"../../RenderSystem/JGMaterial/JGMesh.h"
#include"../../RenderSystem/JGDeviceD.h"
using namespace std;
Mesh2DComponent::Mesh2DComponent()
{
	m_Texture = make_unique<JGTexture>();
	m_Mesh = make_unique<JGMesh>();

}

Mesh2DComponent::~Mesh2DComponent()
{
}

bool Mesh2DComponent::ConstructMesh2D(const std::wstring & TexturePath)
{
	// 텍스쳐 추가
	bool result = m_Texture->Add(JGDeviceD::GetInstance(), TexturePath);
	if (!result)
	{
		return false;
	}
	// 메쉬 만들고
	// 셰이더 만들고
	return true;
}

void Mesh2DComponent::Render()
{
	Motivated2DComponent::Render();
	// JGShaderData 생성 ( 한번 만들어두고 게속 쓸수있도록 만든다.)
	JGShaderData Data;
	// 메쉬 렌더링
	m_Mesh->Render(JGDeviceD::GetInstance(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 셰이더 렌더링
	JGHLSLShaderDevice::GetInstance()->GetShader(m_ShaderName)->Render(nullptr, m_Texture.get());

}
