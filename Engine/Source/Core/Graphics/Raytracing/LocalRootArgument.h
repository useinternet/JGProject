#pragma once
#include "JGCore.h"


namespace JG
{
	class ITexture;
	class IVertexBuffer;
	class IIndexBuffer;
	class LocalRootArgument
	{
		List<jbyte>				  mConstant;
		List<SharedPtr<ITexture>> mTextures;
		SharedPtr<IIndexBuffer>	  mIndexBuffer;
		SharedPtr<IVertexBuffer>  mVertexBuffer;
	public:
		void SetConstant(const void* data, u64 dataSize);
		void SetTextures(const List<SharedPtr<ITexture>>& textures);
		void SetIndexBuffer(SharedPtr<IIndexBuffer>& indexBuffer);
		void SetVertexBuffer(SharedPtr<IVertexBuffer>& vertexBuffer);


		const List<jbyte>& GetConstant() const;
		const List<SharedPtr<ITexture>>& GetTextures() const;
		SharedPtr<IIndexBuffer> GetIndexBuffer() const;
		SharedPtr<IVertexBuffer> GetVertexBuffer() const;

		u64 GetArgumentSize() const;
	};
}