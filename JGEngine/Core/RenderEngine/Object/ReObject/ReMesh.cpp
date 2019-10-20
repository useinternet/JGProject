#include "pch.h"
#include "ReMesh.h"
#include "Object/DxObject/CommandList.h"
using namespace std;

namespace RE
{
	void ReMesh::Draw(CommandList* cmdList, uint32_t instanceCount)
	{
		cmdList->BindDynamicVertexBuffer(0, m_MeshVertexDatas);
		cmdList->BindDynamicIndexBuffer(m_MeshIndexDatas);
		cmdList->SetPrimitiveTopology(m_Topology);
		cmdList->DrawIndexed((uint32_t)m_MeshIndexDatas.size(), instanceCount);
	}
}