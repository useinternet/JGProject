#include<PCH.h>
#include"Mesh.h"
#include"DirectXObjects/ShaderCommonDefines.h"
#include"DirectXToolKit/Shader.h"
using namespace Dx12;
using namespace std;


Mesh::Mesh() :
	m_TopologyType(D3D12_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	m_IndexCount(0)
{

}

Mesh::~Mesh()
{

}

void Mesh::RegisterShaderMacro(GraphicsShader& shader)
{
	if (m_MeshType == MeshType::Skeletal)
	{
		shader.AddMacro(ShaderStage::Vertex, SHADER_MACRO_USE_SKINNED, SHADER_MACRO_DEFINE);
	}
}
void Mesh::Draw(CommandList* commandList, uint32_t instanceCount) const
{
	commandList->SetPrimitiveTopology(m_TopologyType);
	commandList->SetVertexBuffer(m_VertexBuffer);
	commandList->SetIndexBuffer(m_IndexBuffer);
	commandList->DrawIndexed(m_IndexCount, instanceCount);
}