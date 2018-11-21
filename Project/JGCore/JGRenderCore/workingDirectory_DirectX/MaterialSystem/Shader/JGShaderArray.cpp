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
void JGShaderArray::Render(const uint indexCount)
{
	for (auto& iter : m_mShaders)
	{
		iter.second->Render();
	}
	GetDx()->GetContext()->DrawIndexed((UINT)indexCount, 0, 0);
}
void JGShaderArray::Move(JGShaderArray* mover)
{
	mover->m_mShaders = m_mShaders;
	m_mShaders.clear();
}