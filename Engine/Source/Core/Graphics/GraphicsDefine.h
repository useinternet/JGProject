#pragma once
#include "JGCore.h"
#include <dxgi1_6.h>
#include <d3d12.h>




namespace JG
{
#define MAX_RENDERTARGET 8
#define TEXTURE_ID_NULL  0
#define MAIN_GRAPHICS_COMMAND_ID 0

	using TextureID  = u64;
	using BufferID   = u64;
	enum class ETextureFormat
	{
		None,
		R8_Unorm,
		R16_Float,
		R32_Float,
		R16G16_Float,
		R8G8B8A8_Unorm,
		R16G16B16A16_Unorm,
		R11G11B10_Float,
		R16G16B16A16_Float,
		R32G32B32A32_Float,
		R16G16B16A16_Uint,
		R8_Uint,
		R16_Uint,
		R32_Uint,
		R24G8_TYPELESS,
		D24_Unorm_S8_Uint
	};
	inline String TextureFormatToString(ETextureFormat format)
	{
		switch (format)
		{
		case ETextureFormat::R11G11B10_Float:    return "R11G11B10_Float";
		case ETextureFormat::R8_Unorm:			 return "R8_Unrom";
		case ETextureFormat::R32_Float:			 return "R32_Float";
		case ETextureFormat::R16_Float:			 return "R16_Float";
		case ETextureFormat::R16G16_Float:		 return "R16G16_Float";
		case ETextureFormat::R8G8B8A8_Unorm:     return "R8G8B8A8_Unorm";
		case ETextureFormat::R16G16B16A16_Unorm: return "R16G16B16A16_Unorm";
		case ETextureFormat::R32G32B32A32_Float: return "R32G32B32A32_Float";
		case ETextureFormat::R16G16B16A16_Uint:  return "R16G16B16A16_Uint";
		case ETextureFormat::R8_Uint:			 return "R8_Uint";
		case ETextureFormat::R16_Uint:			 return "R16_Uint";
		case ETextureFormat::R32_Uint:			 return "R32_Uint";
		case ETextureFormat::R24G8_TYPELESS:	 return "R24G8_Typeless";
		case ETextureFormat::D24_Unorm_S8_Uint:  return "D24_Unorm_S8_Uint";
		default: return "None";
		}
	}


	inline DXGI_FORMAT ConvertDXGIFormat(ETextureFormat format)
	{
		
		switch (format)
		{
		case ETextureFormat::R11G11B10_Float:    return DXGI_FORMAT_R11G11B10_FLOAT;
		case ETextureFormat::R8_Unorm:			 return DXGI_FORMAT_R8_UNORM;
		case ETextureFormat::R16_Float:			 return DXGI_FORMAT_R16_FLOAT;
		case ETextureFormat::R32_Float:			 return DXGI_FORMAT_R32_FLOAT;
		case ETextureFormat::R16G16_Float:		 return DXGI_FORMAT_R16G16_FLOAT;
		case ETextureFormat::R8G8B8A8_Unorm:     return DXGI_FORMAT_R8G8B8A8_UNORM;
		case ETextureFormat::R16G16B16A16_Unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case ETextureFormat::R16G16B16A16_Float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case ETextureFormat::R32G32B32A32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT; 
		case ETextureFormat::R16G16B16A16_Uint:  return DXGI_FORMAT_R16G16B16A16_UINT;
		case ETextureFormat::R8_Uint:			 return DXGI_FORMAT_R8_UINT;
		case ETextureFormat::R16_Uint:			 return DXGI_FORMAT_R16_UINT;
		case ETextureFormat::R32_Uint:			 return DXGI_FORMAT_R32_UINT;
		case ETextureFormat::R24G8_TYPELESS:	 return DXGI_FORMAT_R24G8_TYPELESS;
		case ETextureFormat::D24_Unorm_S8_Uint:  return	DXGI_FORMAT_D24_UNORM_S8_UINT;
		default:
			JG_LOG_ERROR("This {0} DirectX12 TextureFormat is not supported convert ETextureFormat", TextureFormatToString(format));
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	enum class EResourceState
	{
		Common,
		VertexAndConstantBuffer,
		IndexBuffer,
		RenderTarget,
		UnorderedAccess,
		DepthWrite,
		DepthRead,
		NonePixelShaderResource,
		PixelShaderResource,
		CopyDest,
		CopySource,
		RayTracingAccelerationStructure,
		GenericRead,
	};

	inline D3D12_RESOURCE_STATES ConvertDX12ResourceState(EResourceState state)
	{
		switch (state)
		{
		case EResourceState::Common:					return D3D12_RESOURCE_STATE_COMMON;
		case EResourceState::VertexAndConstantBuffer:	return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		case EResourceState::IndexBuffer:				return D3D12_RESOURCE_STATE_INDEX_BUFFER;
		case EResourceState::RenderTarget:				return D3D12_RESOURCE_STATE_RENDER_TARGET;
		case EResourceState::UnorderedAccess:			return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		case EResourceState::DepthWrite:				return D3D12_RESOURCE_STATE_DEPTH_WRITE;
		case EResourceState::DepthRead:					return D3D12_RESOURCE_STATE_DEPTH_READ;
		case EResourceState::NonePixelShaderResource:	return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		case EResourceState::PixelShaderResource:		return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		case EResourceState::CopyDest:					return D3D12_RESOURCE_STATE_COPY_DEST;
		case EResourceState::CopySource:				return D3D12_RESOURCE_STATE_COPY_SOURCE;
		case EResourceState::RayTracingAccelerationStructure: return D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
		case EResourceState::GenericRead:				return D3D12_RESOURCE_STATE_GENERIC_READ;
		default:
			return D3D12_RESOURCE_STATE_COMMON;

		}
		return D3D12_RESOURCE_STATE_COMMON;
	}


	ENUM_FLAG(ETextureFlags)
		enum class ETextureFlags
	{
		None = 0x00001,
		Allow_RenderTarget = 0x00002,
		Allow_DepthStencil = 0x00004,
		Allow_UnorderedAccessView = 0x00008,
		SRV_TextureCube   = 0x00010,
	};




	struct TextureInfo
	{
		u32 Width  = 0;
		u32 Height = 0;
		u32 PixelPerUnit = 1;
		ETextureFormat Format = ETextureFormat::None;
		ETextureFlags  Flags = ETextureFlags::None;
		u16 MipLevel  = 0;
		u16 ArraySize = 1;


		Color ClearColor;
		f32   ClearDepth = 1.0f;
		u8    ClearStencil = 0;
	};


	class InputLayout;
	struct MeshInfo
	{
		String Name;
		u32 TotalSubMeshCount = 0;
		u32 TotalVertexCount  = 0;
		u32 TotalIndexCount   = 0;

		List<String> SubMeshNames;
		List<u32>    SubMeshVertexCounts;
		List<u32>    SubMeshIndexCounts;

		SharedPtr<InputLayout> InputLayout;
	};
	

	ENUM_FLAG(EShaderFlags)
		enum class EShaderFlags
	{
		None = 0x000,
		Allow_VertexShader   = 0x001,
		Allow_PixelShader    = 0x002,
		Allow_GeometryShader = 0x004,
		Allow_HullShader     = 0x008,
		Allow_DomainShader   = 0x010,
		Allow_ComputeShader = 0x020,
	};


	enum class EShaderDataType
	{
		unknown,
		_bool,
		_int, _int2, _int3, _int4,
		_uint, _uint2, _uint3, _uint4,
		_float, _float2, _float3, _float4,
		_float3x3, _float4x4,

		texture2D, textureCube,
		rwtexture2D,
	};

	inline u64 GetShaderDataTypeSize(EShaderDataType type)
	{
		switch (type)
		{
		case EShaderDataType::_bool: return 1;
		case EShaderDataType::_int:  return 4;
		case EShaderDataType::_int2: return 8;
		case EShaderDataType::_int3: return 12;
		case EShaderDataType::_int4: return 16;
		case EShaderDataType::_uint: return 4;
		case EShaderDataType::_uint2: return 8;
		case EShaderDataType::_uint3: return 12;
		case EShaderDataType::_uint4: return 16;
		case EShaderDataType::_float: return 4;
		case EShaderDataType::_float2: return 8;
		case EShaderDataType::_float3: return 12;
		case EShaderDataType::_float4: return 16;
		case EShaderDataType::_float3x3: return 36;
		case EShaderDataType::_float4x4: return 64;
		default:
			JG_LOG_ERROR("not supported ShaderDataType");
			return 0;
		}
	}
	inline String ShaderDataTypeToString(EShaderDataType type)
	{
		switch (type)
		{
		case EShaderDataType::_bool:	return "bool";
		case EShaderDataType::_int:		return "int";
		case EShaderDataType::_int2:	return "int2";
		case EShaderDataType::_int3:	return "int3";
		case EShaderDataType::_int4:	return "int4";
		case EShaderDataType::_uint:	return "uint";
		case EShaderDataType::_uint2:	return "uint2";
		case EShaderDataType::_uint3:	return "uint3";
		case EShaderDataType::_uint4:	return "uin4";
		case EShaderDataType::_float:	return "float";
		case EShaderDataType::_float2:	return "float2";
		case EShaderDataType::_float3:	return "float3";
		case EShaderDataType::_float4:   return "float4";
		case EShaderDataType::_float3x3: return "float3x3";
		case EShaderDataType::_float4x4: return "float4x4";
		case EShaderDataType::texture2D: return "Texture2D";
		case EShaderDataType::textureCube: return"TextureCube";
		case EShaderDataType::rwtexture2D: return "RWTexture2D";
		default:
			JG_LOG_ERROR("not supported ShaderDataType");
			return "unknown";
		}
	}
	inline EShaderDataType  StringToShaderDataType(const String& type)
	{
		if (type == "bool") return EShaderDataType::_bool;
		else if (type == "int") return EShaderDataType::_int;
		else if (type == "int2") return EShaderDataType::_int2;
		else if (type == "int3") return EShaderDataType::_int3;
		else if (type == "int4") return EShaderDataType::_int4;
		else if (type == "uint") return EShaderDataType::_uint;
		else if (type == "uint2") return EShaderDataType::_uint2;
		else if (type == "uint3") return EShaderDataType::_uint3;
		else if (type == "uint4") return EShaderDataType::_uint4;
		else if (type == "float") return EShaderDataType::_float;
		else if (type == "float2") return EShaderDataType::_float2;
		else if (type == "float3") return EShaderDataType::_float3;
		else if (type == "float4") return EShaderDataType::_float4;
		else if (type == "float3x3") return EShaderDataType::_float3x3;
		else if (type == "float4x4") return EShaderDataType::_float4x4;
		else if (type == "Texture2D") return EShaderDataType::texture2D;
		else if (type == "TextureCube") return EShaderDataType::textureCube;
		else if (type == "RWTexture2D") return EShaderDataType::rwtexture2D;
		else
		{
			return EShaderDataType::unknown;
		}
	}

	enum class EDepthStencilStateTemplate
	{
		Default = 0,
		NoDepth = 1,
		LessEqual  = 2,
	};

	enum class EBlendStateTemplate
	{
		Default = 0,
		Transparent_Default,
	};
	//D3D12_RASTERIZER_DESC
	enum class ERasterizerStateTemplate
	{
		Default = 0,
		Cull_None = 1,
	};

	class DepthStencilState
	{

	};
	class BlendState
	{

	};
	class RasterizerState
	{

	};


	enum class EBufferLoadMethod
	{
		GPULoad,
		CPULoad,
	};
	enum class EGeometryType
	{
		Box,
		Sphere,
		Capsule,
		Line,
	};
	enum class ECommandQueueType
	{
		Graphics,
		Compute
	};

	class ScissorRect
	{
	public:
		i32    Left = 0;
		i32    Top  = 0;
		i32    Right= 0;
		i32    Bottom = 0;

	public:
		ScissorRect(i32 left, i32 top, i32 right, i32 bottom) :
			Left(left), Top(top), Right(right), Bottom(bottom){}
	};


	class Viewport
	{
	public:
		f32 TopLeftX = 0.0f;
		f32 TopLeftY = 0.0f;
		f32 Width    = 0.0f;
		f32 Height   = 0.0f;
		f32 MinDepth = 0.0f;
		f32 MaxDepth = 1.0f;
	public:
		Viewport(f32 w, f32 h) : Width(w), Height(h) {}
		Viewport(f32 topLeftX, f32 topLeftY, f32 w, f32 h) {}
	};



	namespace ShaderDefine
	{
		namespace Template
		{
			constexpr char* Standard2DShader		= "Standard2DShader";
			constexpr char* Standard3DShader		= "Standard3DShader";
			constexpr char* StandardSceneShader		= "StandardSceneShader";
			constexpr char* StandardEditorUIShader  = "StandardEditorUIShader";
			constexpr char* DebugShader				= "DebugShader";
			constexpr char* DirectClosestHitShader   = "DirectClosestHitShader";
			constexpr char* IndirectClosestHitShader = "IndirectClosestHitShader";
		}
		namespace Type
		{
			constexpr char* Resources = "_Resources";
			constexpr char* Function = "_Function";
			constexpr char* Variables = "_Variables";
			constexpr char* Surface   = "_Surface";
			constexpr char* Scene = "_Scene";
		}

		namespace Location
		{
			constexpr char* SurfaceResources = "__PS_SURFACE_RESOURCES_SCRIPT__";
			constexpr char* SurfaceVariables = "__PS_SURFACE_VARIABLES_SCRIPT__";
			constexpr char* SurfaceFunction = "__PS_SURFACE_FUNCTION_SCRIPT__";
			constexpr char* SurfaceContents  = "__PS_SURFACE_CONTENTS_SCRIPT__";


			constexpr char* SceneResources = "__PS_SCENE_RESOURCES_SCRIPT__";
			constexpr char* SceneVariables = "__PS_SCENE_VARIABLES_SCRIPT__";
			constexpr char* SceneFunction = "__PS_SCENE_FUNCTION_SCRIPT__";
			constexpr char* SceneContents  = "__PS_SCENE_CONTENTS_SCRIPT__";
		}

		namespace RayTracing
		{
			constexpr char* RayGen = "RayGeneration";
			constexpr char* DirectNullHit = "DirectClosestHit";
			constexpr char* DirectMiss = "DirectMiss";


			constexpr char* IndirectNullHit = "IndirectClosestHit";
			constexpr char* IndirectMiss = "IndirectMiss";

			constexpr char* ShadowHit = "ShadowHit";
			constexpr char* ShadowMiss = "ShadowMiss";

			constexpr char* DirectNullHitGroup   = "DirectNullHitGroup";
			constexpr char* IndirectNullHitGroup = "IndirectNullHitGroup";
			constexpr char* ShadowHitGroup       = "ShadowHitGroup";
		}
	}
	namespace HLSL
	{
		using token = char;
		namespace Token
		{
			constexpr token* Struct  = "struct ";
			constexpr token* CBuffer = "cbuffer ";
			constexpr token* StructuredBuffer = "StructuredBuffer";
			constexpr token* Texture2D   = "Texture2D";
			constexpr token* TextureCube = "TextureCube";
			constexpr token* RWStructuredBuffer = "RWStructuredBuffer";
			constexpr token* RWTexture2D  = "RWTexture2D<";
			constexpr token* ByteAddressBuffer = "ByteAddressBuffer";
			constexpr token* RWByteAddressBuffer = "RWByteAddressBuffer";
			constexpr token* SamplerState = "SamplerState";
			constexpr token* SamplerComparisonState = "SamplerComparisonState";
		}
		namespace RegisterNumber
		{
			constexpr int RenderPassRegisterNumber = 0;
			constexpr int ObjectRegisterNumber     = 1;
			constexpr int MaterialRegisterNumber   = 2;
		}
		namespace RegisterSpace
		{
			constexpr int Texture2DRegisterSpace   = 0;
			constexpr int TextureCubeRegisterSpace = 1;
		}

		constexpr char* VSEntry = "vs_main";
		constexpr char* DSEntry = "ds_main";
		constexpr char* HSEntry = "hs_main";
		constexpr char* GSEntry = "gs_main";
		constexpr char* PSEntry = "ps_main";
		constexpr char* CSEntry = "cs_main";
		constexpr char* VSTarget = "vs_5_1";
		constexpr char* DSTarget = "ds_5_1";
		constexpr char* HSTarget = "hs_5_1";
		constexpr char* GSTarget = "gs_5_1";
		constexpr char* PSTarget = "ps_5_1";
		constexpr char* CSTarget = "cs_6_0";
		constexpr char* RTTarget = "lib_6_3";
		constexpr char* ClosestHitEntry = "__ClosestHit_EntryPoint__";
		constexpr char* MissEntry       = "__Miss_EntryPoint__";


		inline String ShaderDataTypeToHLSLCode(EShaderDataType type)
		{
			switch (type)
			{
			case EShaderDataType::_bool:	return "bool";
			case EShaderDataType::_int:		return "int";
			case EShaderDataType::_int2:	return "int2";
			case EShaderDataType::_int3:	return "int3";
			case EShaderDataType::_int4:	return "int4";
			case EShaderDataType::_uint:	return "uint";
			case EShaderDataType::_uint2:	return "uint2";
			case EShaderDataType::_uint3:	return "uint3";
			case EShaderDataType::_uint4:	return "uin4";
			case EShaderDataType::_float:	return "float";
			case EShaderDataType::_float2:	return "float2";
			case EShaderDataType::_float3:	return "float3";
			case EShaderDataType::_float4:   return "float4";
			case EShaderDataType::_float3x3: return "float3x3";
			case EShaderDataType::_float4x4: return "float4x4";
			default:
				JG_LOG_ERROR("not supported ShaderDataType");
				return "unknown";
			}
		}



	}
}