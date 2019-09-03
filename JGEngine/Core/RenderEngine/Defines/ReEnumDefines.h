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


	namespace ShaderEnum
	{
		enum EShaderVisible
		{
			Visible_Vertex,
			Visible_Hull,
			Visible_Domain,
			Visible_Geometry,
			Visible_Pixel
		};
		enum EShaderKeyword
		{
			_const,
			_static,
		};
		enum EShaderDataType
		{
			_bool,
			_float,
			_float2,
			_float3,
			_float4,
			_int,
			_int2,
			_int3,
			_int4,
			_uint,
			_uint2,
			_uint3,
			_uint4,
			_matrix3x3,
			_matrix4x4
		};
		inline std::string ShaderDataTypeToString(EShaderDataType type)
		{
			switch (type)
			{
			case _bool:  return "bool";
			case _float:  return "float";
			case _float2: return "float2";
			case _float3: return "float3";
			case _float4: return "float4";
			case _int: return "int";
			case _int2: return "int2";
			case _int3: return "int3";
			case _int4: return "int4";
			case _uint: return "uint";
			case _uint2: return "uint2";
			case _uint3: return "uint3";
			case _uint4: return "uint4";
			case _matrix3x3: return "matrix3x3";
			case _matrix4x4: return "matrix4x4";
			}
			return "none";
		}
		enum EShaderBindingDataType
		{
			SamplerState,
			ConstantBuffer,
			Constants,
			StructedBuffer,
			Texture2D,
			Texture2DArray,
			TextureCube,
			RWTexture2D,
			RWTexture2DArray,
		};
	}

}