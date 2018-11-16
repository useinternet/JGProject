#include"SRSMaterial.h"
#include"DirectX/DirectX.h"

using namespace std;
using namespace JGRC;

SRSMaterial::SRSMaterial()
{
	m_wvpMatrix = make_unique<jgMatrix4x4>();
}
SRSMaterial::~SRSMaterial()
{

}
void  SRSMaterial::SetMesh(Mesh* mesh)
{
	Material::SetMesh(mesh);
}
real* SRSMaterial::GetParam(const std::string& paramName)
{
	return Material::GetParam(paramName);
}
void  SRSMaterial::SetParam(const std::string& paramName, void* Data)
{
	Material::SetParam(paramName,Data);
}
uint  SRSMaterial::GetParamCount(const std::string& paramName)
{
	return Material::GetParamCount(paramName);
}
void SRSMaterial::Render(ID3D11ShaderResourceView* WorldPos, ID3D11ShaderResourceView* Albedo,
	ID3D11ShaderResourceView* Normal, ID3D11ShaderResourceView* Depth)
{
	bool result = true;
	result = Material::MeshRendering();
	if (!result) return;
	result = Material::WriteConstantBuffer();
	if (!result) return;
	Material::m_DirectX->GetContext()->PSSetShaderResources(0, 1, &WorldPos);
	Material::m_DirectX->GetContext()->PSSetShaderResources(1, 1, &Albedo);
	Material::m_DirectX->GetContext()->PSSetShaderResources(2, 1, &Normal);
	Material::m_DirectX->GetContext()->PSSetShaderResources(3, 1, &Depth);
	result = Material::InputShader_Sampler();
	if (!result) return;
	result = Draw();
	if (!result) return;
}