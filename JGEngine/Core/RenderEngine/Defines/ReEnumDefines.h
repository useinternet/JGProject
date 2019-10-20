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
		enum EShaderParam
		{
			// Vertex
			InstanceID
		};
		inline std::string ShaderParamToString(EShaderParam param)
		{
			switch (param)
			{
			case InstanceID:
				return "SV_InstanceID";
			}
			return "";
		}
		enum EShaderVisible
		{
			Visible_Vertex = 0x0001,
			Visible_Hull = 0x0002,
			Visible_Domain = 0x0004,
			Visible_Geometry = 0x0008,
			Visible_Pixel = 0x0016,
			Visible_All = 0xfffff
		};
		inline EShaderVisible operator|(EShaderVisible v1, EShaderVisible v2)
		{
			return (EShaderVisible)((int)v1 | (int)v2);
		}
		inline D3D12_SHADER_VISIBILITY ShaderVisibleToDx12(EShaderVisible visible)
		{
			switch (visible)
			{
			case Visible_Vertex:
				return D3D12_SHADER_VISIBILITY_VERTEX;
			case Visible_Hull:
				return D3D12_SHADER_VISIBILITY_HULL;
			case Visible_Domain:
				return D3D12_SHADER_VISIBILITY_DOMAIN;
			case Visible_Geometry:
				return D3D12_SHADER_VISIBILITY_GEOMETRY;
			case Visible_Pixel:
				return D3D12_SHADER_VISIBILITY_PIXEL;
			default:
				return D3D12_SHADER_VISIBILITY_ALL;
			}
			return D3D12_SHADER_VISIBILITY_ALL;
		}
		enum EShaderKeyword
		{
			_none,
			_const,
			_static,
		};
		inline std::string ShaderKeywordToString(EShaderKeyword keyword)
		{
			switch (keyword)
			{
			case _none:
				return "";
			case _const:
				return "const";
			case _static:
				return "static";
			}
			return "none";
		}
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
		inline size_t ShaderDataTypeToSize(EShaderDataType type)
		{
			switch (type)
			{
			case _bool:  return sizeof(bool);
			case _float:  return sizeof(float);
			case _float2: return sizeof(float) * 2;
			case _float3: return sizeof(float) * 3;
			case _float4: return sizeof(float) * 4;
			case _int: return sizeof(int);
			case _int2: return sizeof(int) * 2;
			case _int3: return sizeof(int) * 3;
			case _int4: return sizeof(int) * 4;
			case _uint: return sizeof(unsigned int);
			case _uint2: return sizeof(unsigned int) * 2;
			case _uint3: return sizeof(unsigned int) * 3;
			case _uint4: return sizeof(unsigned int) * 4;
			case _matrix3x3: return sizeof(float) * 9;
			case _matrix4x4: return sizeof(float) * 16;
			}
			return 0;
		}
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
			case _matrix3x3: return "float3x3";
			case _matrix4x4: return "float4x4";
			}
			return "none";
		}
		inline DXGI_FORMAT ShaderDataTypeToFormat(EShaderDataType type)
		{
			switch (type)
			{
			case _bool:  return DXGI_FORMAT_R8_SINT;
			case _float:  return DXGI_FORMAT_R32_FLOAT;
			case _float2: return DXGI_FORMAT_R32G32_FLOAT;
			case _float3: return DXGI_FORMAT_R32G32B32_FLOAT;
			case _float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
			case _int: return DXGI_FORMAT_R32_SINT;
			case _int2: return DXGI_FORMAT_R32G32_SINT;
			case _int3: return DXGI_FORMAT_R32G32B32_SINT;
			case _int4: return DXGI_FORMAT_R32G32B32A32_SINT;
			case _uint: return DXGI_FORMAT_R32_UINT;
			case _uint2: return DXGI_FORMAT_R32G32_UINT;
			case _uint3: return DXGI_FORMAT_R32G32B32_UINT;
			case _uint4: return DXGI_FORMAT_R32G32B32A32_UINT;
			}
			return DXGI_FORMAT_UNKNOWN;
		
		}
		inline EShaderDataType FormatToShaderDataType(DXGI_FORMAT format)
		{
			switch (format)
			{
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R8_SNORM:
			case DXGI_FORMAT_R16_SNORM:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_R32_FLOAT:  return _float;

			case DXGI_FORMAT_R8G8_SNORM:
			case DXGI_FORMAT_R8G8_UNORM:
			case DXGI_FORMAT_R16G16_SNORM:
			case DXGI_FORMAT_R16G16_UNORM:
			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R32G32_FLOAT: return _float2;

			case DXGI_FORMAT_R32G32B32_FLOAT: return _float3;

			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_FLOAT: return _float4;

			case DXGI_FORMAT_R8_SINT:
			case DXGI_FORMAT_R16_SINT:
			case DXGI_FORMAT_R32_SINT: return _int;

			case DXGI_FORMAT_R8G8_SINT:
			case DXGI_FORMAT_R16G16_SINT:
			case DXGI_FORMAT_R32G32_SINT: return _int2;

			case DXGI_FORMAT_R32G32B32_SINT: return _int3;

			case DXGI_FORMAT_R8G8B8A8_SINT:
			case DXGI_FORMAT_R16G16B16A16_SINT:
			case DXGI_FORMAT_R32G32B32A32_SINT: return _int4;

			case DXGI_FORMAT_R8_UINT:
			case DXGI_FORMAT_R16_UINT:
			case DXGI_FORMAT_R32_UINT: return _uint;

			case DXGI_FORMAT_R8G8_UINT:
			case DXGI_FORMAT_R16G16_UINT:
			case DXGI_FORMAT_R32G32_UINT: return _uint2;

			case DXGI_FORMAT_R32G32B32_UINT: return _uint3;

			case DXGI_FORMAT_R8G8B8A8_UINT:
			case DXGI_FORMAT_R16G16B16A16_UINT:
			case DXGI_FORMAT_R32G32B32A32_UINT: return _uint4;
			default: return _bool;
			}
		}
		enum EShaderResourceType
		{
			Texture2D,
			TextureCube,
			RWTexture2D,
			RWTextureCube
		};
		inline std::string ShaderResourceTypeToString(EShaderResourceType type)
		{
			switch (type)
			{
			case Texture2D:
				return "Texture2D";
			case TextureCube:
				return "TextureCube";
			case RWTexture2D:
				return "RWTexture2D";
			case RWTextureCube:
				return "RWTextureCube";
			default:
				return "";
			}
		}
	}

}