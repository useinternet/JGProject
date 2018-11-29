#include"JGShaderArray.h"
#include"DirectX/DirectX.h"
using namespace JGRC;
using namespace std;


JGShaderArray::JGShaderArray()
{

}
JGShaderArray::~JGShaderArray()
{
	for (auto& shader : m_mShaders)
	{
		GetDx()->DeleteObject(shader.second);
	}
}
JGShader* JGShaderArray::Get(const EShaderType type)
{
	return m_mShaders[type];
}
void JGShaderArray::AddShader(JGShader* shader)
{
	m_mShaders.insert(ShaderArrayPair(shader->GetType(), shader));
}
void JGShaderArray::Render(const uint indexCount, const EShaderRenderType type)
{
	GetDx()->GetContext()->PSSetShader(nullptr, nullptr, 0);
	GetDx()->GetContext()->VSSetShader(nullptr, nullptr, 0);
	GetDx()->GetContext()->HSSetShader(nullptr, nullptr, 0);
	GetDx()->GetContext()->DSSetShader(nullptr, nullptr, 0);
	GetDx()->GetContext()->GSSetShader(nullptr, nullptr, 0);

	for (auto& iter : m_mShaders)
	{
		iter.second->Render();
	}
	switch (type)
	{
	case EShaderRenderType::DrawIndexed:
		GetDx()->GetContext()->DrawIndexed((UINT)indexCount, 0, 0);
		break;
	case EShaderRenderType::Draw:
		GetDx()->GetContext()->Draw((UINT)indexCount, 0);
		break;
	}
}
void JGShaderArray::Move(JGShaderArray* mover)
{
	mover->m_mShaders = m_mShaders;
	m_mShaders.clear();
}