#include "pch.h"
#include "LocalRootArgument.h"


namespace JG
{
	void LocalRootArgument::SetConstant(const void* data, u64 dataSize)
	{
		mConstant.resize(256);
		memcpy(mConstant.data(), data, dataSize);
	}
	void LocalRootArgument::SetTextures(const List<SharedPtr<ITexture>>& textures)
	{
		mTextures = textures;
	}
	void LocalRootArgument::SetIndexBuffer(SharedPtr<IIndexBuffer>& indexBuffer)
	{
		mIndexBuffer = indexBuffer;
	}
	void LocalRootArgument::SetVertexBuffer(SharedPtr<IVertexBuffer>& vertexBuffer)
	{
		mVertexBuffer = vertexBuffer;
	}
	const List<jbyte>& LocalRootArgument::GetConstant() const
	{
		return mConstant;
	}
	const List<SharedPtr<ITexture>>& LocalRootArgument::GetTextures() const
	{
		return mTextures;
	}
	SharedPtr<IIndexBuffer> LocalRootArgument::GetIndexBuffer() const
	{
		return mIndexBuffer;
	}
	SharedPtr<IVertexBuffer> LocalRootArgument::GetVertexBuffer() const
	{
		return mVertexBuffer;
	}
	u64 LocalRootArgument::GetArgumentSize() const
	{
		u64 result = mConstant.size();
		u64 handleCount = 3 + mTextures.size();
		result += (sizeof(u64) * handleCount);
		return result;
	}
}