#pragma once
#include "Graphics/Mesh.h"
#include "Graphics/InputLayout.h"
#include "Graphics/Resource.h"

namespace JG
{
	class IBottomLevelAccelerationStructure;
	class DirectX12SubMesh : public ISubMesh
	{
		SharedPtr<IVertexBuffer> mVertexBuffer;
		SharedPtr<IIndexBuffer>  mIndexBuffer;
		String mName;
		u64	   mInstanceCount = 1;


		SharedPtr<IBottomLevelAccelerationStructure> mBottomAS;
	public:
		virtual ~DirectX12SubMesh() = default;
	public:
		virtual void SetVertexBuffer(SharedPtr<IVertexBuffer> vertexBuffer) override;
		virtual void SetIndexBuffer(SharedPtr<IIndexBuffer> indexBuffer) override;
		virtual SharedPtr<IVertexBuffer> GetVertexBuffer() const override;
		virtual SharedPtr<IIndexBuffer>  GetIndexBuffer() const override;
		virtual SharedPtr<IBottomLevelAccelerationStructure> GetBottomLevelAS() const override ;
		virtual void SetBottomLevelAccelerationStructure(SharedPtr<IBottomLevelAccelerationStructure> bottomAS) override;
	public:
		virtual void SetName(const String& name) override;
		virtual const String& GetName() const override;
		virtual u32 GetIndexCount() const override;
		virtual void SetInstanceCount(u32 cnt) override;
		virtual u32 GetInstanceCount() const override;
		virtual bool IsValid() const override;
	};
	class DirectX12Mesh : public IMesh
	{
		List<SharedPtr<ISubMesh>> mSubMeshList;
		SharedPtr<InputLayout>    mInputLayout;
		JBBox mBoundingBox;
		String mName;
	public:
		virtual ~DirectX12Mesh() = default;
	public:
		virtual void AddMesh(SharedPtr<ISubMesh> subMesh) override;
		virtual void SetBoundingBox(const JBBox& boundingBox) override;
		virtual void SetInputLayout(SharedPtr<InputLayout> inputLayout) override;


		virtual SharedPtr<InputLayout> GetInputLayout() const override;
		virtual SharedPtr<ISubMesh>    GetSubMesh(i32 index) override;
		virtual u64					   GetSubMeshCount() const override;
		virtual const JBBox&		   GetBoundingBox() const override;

		virtual void	      SetName(const String& name) override;
		virtual const String& GetName() override;
		virtual bool IsValid() const override;
	};
}