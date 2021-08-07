#include "pch.h"
#include "DirectX12Mesh.h"


#include "DirectX12API.h"
#include "Utill/PipelineState.h"
#include "Utill/CommandList.h"

namespace JG
{
	void DirectX12SubMesh::SetVertexBuffer(SharedPtr<IVertexBuffer> vertexBuffer)
	{
		mVertexBuffer = vertexBuffer;
	}

	void DirectX12SubMesh::SetIndexBuffer(SharedPtr<IIndexBuffer> indexBuffer)
	{
		mIndexBuffer = indexBuffer;
	}

	void DirectX12SubMesh::SetName(const String& name)
	{
		mName = name;
	}
	const String& DirectX12SubMesh::GetName() const
	{
		return mName;
	}
	u32 DirectX12SubMesh::GetIndexCount() const
	{
		return mIndexBuffer->GetIndexCount();
	}
	bool DirectX12SubMesh::Bind()
	{
		if (mVertexBuffer == nullptr)
		{
			return false;
		}
		if (mIndexBuffer == nullptr)
		{
			return false;
		}
		// vertexBuffer, IndexBuffer ���ε�
		auto commandList = DirectX12API::GetGraphicsCommandList(GetCommandID());
		commandList->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		auto dx12VBuffer = static_cast<DirectX12VertexBuffer*>(mVertexBuffer.get());
		dx12VBuffer->Bind();
		commandList->FlushVertexBuffer();



		auto dx12IBuffer = static_cast<DirectX12IndexBuffer*>(mIndexBuffer.get());
		dx12IBuffer->Bind();

		return true;
	}


	void DirectX12Mesh::AddMesh(SharedPtr<ISubMesh> subMesh)
	{
		if (subMesh == nullptr)
		{
			return;
		}
		mSubMeshList.push_back(subMesh);
	}

	void DirectX12Mesh::SetBoundingBox(const JBBox& boundingBox)
	{
		mBoundingBox = boundingBox;
	}

	void DirectX12Mesh::SetInputLayout(SharedPtr<InputLayout> inputLayout)
	{
		mInputLayout = inputLayout;
	}

	SharedPtr<ISubMesh> DirectX12Mesh::GetSubMesh(i32 index)
	{
		if (mSubMeshList.size() <= index) return nullptr;
		return mSubMeshList[index];
	}

	u64 DirectX12Mesh::GetSubMeshCount() const
	{
		return mSubMeshList.size();
	}

	const JBBox& DirectX12Mesh::GetBoundingBox() const
	{
		return mBoundingBox;
	}



	void DirectX12Mesh::SetName(const String& name)
	{
		mName = name;
	}

	const String& DirectX12Mesh::GetName()
	{
		return mName;
	}
	bool DirectX12Mesh::Bind()
	{
		if (mInputLayout == nullptr)
		{
			return false;
		}
		auto pso = DirectX12API::GetGraphicsPipelineState();
		pso->BindInputLayout(*mInputLayout);
		pso->SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		return true;
	}

}