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
			ObjectCB()
			{
				DirectX::XMStoreFloat4x4(&World, DirectX::XMMatrixIdentity());
			}
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
			float    FarZ;
			float    NearZ;
			uint32_t LightCount;
			float2   Padding;

			void Set(const Camera& camera);
		};
		struct LightCB
		{
			enum EType{
				Directional, Spot, Point
			};
			uint32_t Type;
			float3   Direction;
			float3   Color;
			float    Intensity;
			float    Bias = 0.000011f;
			float3   Position;
			float    AttConstant1;
			float    AttConstant2;
			float    Range;
			float    padding;
			float    InCos;
			float    OutCos;
			float2   padding2;
		};
	}
}