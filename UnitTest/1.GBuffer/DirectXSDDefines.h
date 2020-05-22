#pragma once
#include "DirectXApi.h"

namespace DX12
{

	namespace DXSD
	{
		__SD_DEFINES_START__
		struct SDStaticObject
		{
			float4x4 world;
		};
		struct SDSkinnedObject
		{
			float4x4 world;
			float4x4 animTransform[128];
		};
		struct SDPaperObject
		{
			float4x4 world;
		};
		__SD_DEFINES_END__
	}

	namespace DXCB
	{
		__SCB_DEFINES_START__
		__SCB__ CBCamera
		{
			float4x4 view;
			float4x4 proj;
			float4x4 viewProj;
			float4x4 invView;
			float4x4 invProj;
			float4x4 invViewProj;


			float lensWidth;
			float lensHeight;
			float farZ;
			float nearZ;
		};
		__SCB_DEFINES_END__
	}


	
}