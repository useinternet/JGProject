#pragma once
#include"Dx12Include.h"
namespace GR
{
	
	namespace Dx12
	{
		class Camera;
		struct ObjectCB
		{
			float4x4 World;
		};
		struct PassCB
		{
			float4x4 ViewProj;
			float4x4 View;
			float4x4 Proj;
			float4x4 InvViewProj;
			float4x4 InvView;
			float4x4 InvProj;
			float3   EyePos;
			float    padding;

			void Set(const Camera& camera);
		};

	}
}