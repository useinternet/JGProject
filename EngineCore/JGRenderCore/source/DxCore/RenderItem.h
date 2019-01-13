#pragma once
#include"Data/JGMaterial.h"

namespace JGRC
{


	typedef struct RCORE_EXPORT RenderItem
	{
		RenderItem() = default;

		DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
		int UpdateNotify = CPU_FRAMERESOURCE_NUM;
		MeshGeometry* Geo = nullptr;
		JGMaterial*     Mat = nullptr;

		D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		DirectX::BoundingBox Bounds;
		UINT ObjCBIndex = -1;
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		int BaseVertexLocation  = 0;

		// юс╫ц
		bool Visible = true;
	}RenderItem;
}