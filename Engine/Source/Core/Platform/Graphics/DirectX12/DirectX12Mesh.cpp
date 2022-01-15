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

	SharedPtr<IVertexBuffer> DirectX12SubMesh::GetVertexBuffer() const
	{
		return mVertexBuffer;
	}

	SharedPtr<IIndexBuffer> DirectX12SubMesh::GetIndexBuffer() const
	{
		return mIndexBuffer;
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
	void DirectX12SubMesh::SetInstanceCount(u32 cnt)
	{
		mInstanceCount = cnt;
	}
	u32 DirectX12SubMesh::GetInstanceCount() const
	{
		return mInstanceCount;
	}

	bool DirectX12SubMesh::IsValid() const
	{
		return mVertexBuffer && mVertexBuffer->IsValid() && mIndexBuffer && mIndexBuffer->IsValid();
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

	SharedPtr<InputLayout> DirectX12Mesh::GetInputLayout() const
	{
		return mInputLayout;
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
	bool DirectX12Mesh::IsValid() const
	{
		bool result = true;


		for (auto& subMesh : mSubMeshList)
		{
			if (subMesh->IsValid() == false)
			{
				result = false;
				break;
			}
		}
		return mInputLayout != nullptr && result;
	}

}