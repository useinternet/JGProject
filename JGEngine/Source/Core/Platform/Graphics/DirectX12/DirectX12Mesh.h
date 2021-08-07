#pragma once
#include "Graphics/Mesh.h"
#include "Graphics/InputLayout.h"
#include "Graphics/Resource.h"

namespace JG
{
	class DirectX12SubMesh : public ISubMesh
	{
		SharedPtr<IVertexBuffer> mVertexBuffer;
		SharedPtr<IIndexBuffer> mIndexBuffer;
		String mName;

	public:
		virtual void SetVertexBuffer(SharedPtr<IVertexBuffer> vertexBuffer) override;
		virtual void SetIndexBuffer(SharedPtr<IIndexBuffer> indexBuffer) override;

	public:
		virtual void SetName(const String& name) override;
		virtual const String& GetName() const override;
		virtual u32 GetIndexCount() const override;
		virtual bool Bind() override;
	};
	class DirectX12Mesh : public IMesh
	{

		List<SharedPtr<ISubMesh>> mSubMeshList;
		SharedPtr<InputLayout> mInputLayout;
		JBBox mBoundingBox;
		String mName;
	public:
		virtual void AddMesh(SharedPtr<ISubMesh> subMesh) override;
		virtual void SetBoundingBox(const JBBox& boundingBox) override;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout) override;
		virtual SharedPtr<ISubMesh> GetSubMesh(i32 index) override;
		virtual u64 GetSubMeshCount() const override;
		virtual const JBBox& GetBoundingBox() const override;

		virtual void SetName(const String& name) override;
		virtual const String& GetName() override;

		virtual bool Bind() override;
	};
}