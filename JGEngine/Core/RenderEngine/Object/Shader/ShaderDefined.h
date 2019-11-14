#pragma once




namespace RE
{
	namespace JGShader
	{
		enum EShaderData
		{
			_int,
			_int2,
			_int3,
			_int4,
			_uint,
			_uint2,
			_uint3,
			_uint4,
			_float,
			_float2,
			_float3,
			_float4,
			_matrix4x4,
			_struct,
		};
		enum EShaderBindData
		{
			ConstantBuffer,
			StructuredBuffer,
			Texture2D,
			TextureCube,
			SamplerState
		};
		enum EShaderBindDataState
		{
			ReadOnly,
			ReadWrite
		};
		enum EShaderParameter
		{
			Vertex_InstanceID,

		};




		inline std::string ToString(EShaderData sdtype)
		{
			switch (sdtype)
			{
			case _int:       return "int";
			case _int2:      return "int2";
			case _int3:      return "int3";
			case _int4:      return "int4";
			case _uint:      return "uint";
			case _uint2:     return "uint2";
			case _uint3:     return "uint3";
			case _uint4:     return "uint4";
			case _float:     return "float";
			case _float2:    return "float2";
			case _float3:    return "float3";
			case _float4:    return "float4";
			case _matrix4x4: return "matrix4x4";
			}
			return "";
		}

		inline std::string ToString(EShaderParameter param)
		{
			switch (param)
			{
			case Vertex_InstanceID:
				return "instanceID : SV_InstanceID";
			}
			return "";
		}


		inline EShaderData ToShaderDataType(DXGI_FORMAT format)
		{
			switch (format)
			{
			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_R8_SNORM:
			case DXGI_FORMAT_R16_SNORM:
			case DXGI_FORMAT_R16_UNORM:
				return _float;
			case DXGI_FORMAT_R8G8_SNORM:
			case DXGI_FORMAT_R8G8_UNORM:
			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R16G16_SNORM:
			case DXGI_FORMAT_R16G16_UNORM:
			case DXGI_FORMAT_R32G32_FLOAT:
				return _float2;
			case DXGI_FORMAT_R32G32B32_FLOAT:
				return _float3;
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
				return _float4;

			case DXGI_FORMAT_R8_UINT:
			case DXGI_FORMAT_R16_UINT:
			case DXGI_FORMAT_R32_UINT:
				return _uint;
			case DXGI_FORMAT_R8G8_UINT:
			case DXGI_FORMAT_R16G16_UINT:
			case DXGI_FORMAT_R32G32_UINT:
				return _uint2;
			case DXGI_FORMAT_R32G32B32_UINT:
				return _uint3;
			case DXGI_FORMAT_R8G8B8A8_UINT:
			case DXGI_FORMAT_R16G16B16A16_UINT:
			case DXGI_FORMAT_R32G32B32A32_UINT:
				return _uint4;

			case DXGI_FORMAT_R8_SINT:
			case DXGI_FORMAT_R16_SINT:
			case DXGI_FORMAT_R32_SINT:
				return _int;
			case DXGI_FORMAT_R8G8_SINT:
			case DXGI_FORMAT_R16G16_SINT:
			case DXGI_FORMAT_R32G32_SINT:
				return _int2;
			case DXGI_FORMAT_R32G32B32_SINT:
				return _int3;
			case DXGI_FORMAT_R8G8B8A8_SINT:
			case DXGI_FORMAT_R16G16B16A16_SINT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
				return _int4;
			}

			return (EShaderData)-1;
		}
		inline uint32_t GetDataSize(EShaderData sdtype)
		{
			switch (sdtype)
			{
			case _int:
			case _uint:
			case _float:
				return 4;
			case _uint2:
			case _int2:
			case _float2:
				return 8;
			case _uint3:
			case _int3:
			case _float3:
				return 12;
			case _uint4:
			case _int4:
			case _float4:
				return 16;
			case _matrix4x4:
				return 64;
			}
			return 0;
		}



		inline ShaderType GetShaderTypeByShaderParam(EShaderParameter param)
		{
			switch (param)
			{
			case Vertex_InstanceID:
				return ShaderType::Vertex;
			}
			return ShaderType::Vertex;
		}


	}
}