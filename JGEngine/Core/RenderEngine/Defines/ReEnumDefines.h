#pragma once
#include <d3d12.h>


namespace RE
{
	enum
	{
		ResourceBarrier_All_Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES
	};

	enum class ShaderType
	{
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Compute
	};


}