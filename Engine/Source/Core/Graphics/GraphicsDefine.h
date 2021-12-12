#pragma once
#include "JGCore.h"
#include <dxgi1_6.h>





namespace JG
{
#define MAX_RENDERTARGET 8
#define TEXTURE_ID_NULL  0
#define MAIN_GRAPHICS_COMMAND_ID 1

	using TextureID  = u64;
	using BufferID   = u64;
	enum class ETextureFormat
	{
		None,
		R8G8B8A8_Unorm,
		R16G16B16A16_Unorm,
		R32G32B32A32_Float,
		D24_Unorm_S8_Uint
	};
	inline String TextureFormatToString(ETextureFormat format)
	{
		switch (format)
		{
		case ETextureFormat::R8G8B8A8_Unorm:     return "R8G8B8A8_Unorm";
		case ETextureFormat::R16G16B16A16_Unorm: return "R16G16B16A16_Unorm";
		case ETextureFormat::R32G32B32A32_Float: return  "R32G32B32A32_Float";
		case ETextureFormat::D24_Unorm_S8_Uint:  return "D24_Unorm_S8_Uint";

		default: return "None";
		}
	}


	inline DXGI_FORMAT ConvertDXGIFormat(ETextureFormat format)
	{

		switch (format)
		{
		case ETextureFormat::R8G8B8A8_Unorm:     return DXGI_FORMAT_R8G8B8A8_UNORM;
		case ETextureFormat::R16G16B16A16_Unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case ETextureFormat::R32G32B32A32_Float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case ETextureFormat::D24_Unorm_S8_Uint:  return	DXGI_FORMAT_D24_UNORM_S8_UINT;
		default:
			JG_CORE_ERROR("This {0} DirectX12 TextureFormat is not supported convert ETextureFormat", TextureFormatToString(format));
			return DXGI_FORMAT_UNKNOWN;
		}
	}






	ENUM_FLAG(ETextureFlags)
		enum class ETextureFlags
	{
		None = 0x00001,
		Allow_RenderTarget = 0x00002,
		Allow_DepthStencil = 0x00004,
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

		texture2D,
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
			JG_CORE_CRITICAL("not supported ShaderDataType");
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
		default:
			JG_CORE_CRITICAL("not supported ShaderDataType");
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
		else
		{
			return EShaderDataType::unknown;
		}
	}

	enum class ESamplerStateTemplate
	{
		Unknown,
		Point_Wrap,
		Point_Clamp,
		Point_Border,
		Point_Mirror,
		Point_MirrorOnce,
		Linear_Wrap,
		Linear_Clamp,
		Linear_Border,
		Linear_Mirror,
		Linear_MirrorOnce,
		Anisotropic_Wrap,
		Anisotropic_Clamp,
		Anisotropic_Border,
		Anisotropic_Mirror,
		Anisotropic_MirrorOnce,
	};

	inline ESamplerStateTemplate StringToSamplerStateTemplate(const String& _template)
	{
		if (_template == "Point_Wrap") return ESamplerStateTemplate::Point_Wrap;
		else if (_template == "Point_Clamp") return ESamplerStateTemplate::Point_Clamp;
		else if (_template == "Point_Border") return ESamplerStateTemplate::Point_Border;
		else if (_template == "Point_Mirror") return ESamplerStateTemplate::Point_Mirror;
		else if (_template == "Point_MirrorOnce") return ESamplerStateTemplate::Point_MirrorOnce;
		else if (_template == "Linear_Wrap") return ESamplerStateTemplate::Linear_Wrap;
		else if (_template == "Linear_Clamp") return ESamplerStateTemplate::Linear_Clamp;
		else if (_template == "Linear_Border") return ESamplerStateTemplate::Linear_Border;
		else if (_template == "Linear_Mirror") return ESamplerStateTemplate::Linear_Mirror;
		else if (_template == "Linear_MirrorOnce") return ESamplerStateTemplate::Linear_MirrorOnce;
		else if (_template == "Anisotropic_Wrap") return ESamplerStateTemplate::Anisotropic_Wrap;
		else if (_template == "Anisotropic_Clamp") return ESamplerStateTemplate::Anisotropic_Clamp;
		else if (_template == "Anisotropic_Border") return ESamplerStateTemplate::Anisotropic_Border;
		else if (_template == "Anisotropic_Mirror") return ESamplerStateTemplate::Anisotropic_Mirror;
		else if (_template == "Anisotropic_MirrorOnce") return ESamplerStateTemplate::Anisotropic_MirrorOnce;
		else return ESamplerStateTemplate::Unknown;
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
	enum class EComputerState
	{
		Run,
		Compelete,
	};
	enum class EReadBackBufferState
	{
		Reading,
		ReadCompelete,
	};
	enum class EGeometryType
	{
		Box,
		Sphere,
		Capsule,
		Line,
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
			constexpr char* Standard2DShader = "Standard2DShader";
			constexpr char* Standard3DShader = "Standard3DShader";
			constexpr char* DebugShader = "DebugShader";
		}

		namespace Standard2D
		{
			constexpr char* ViewProj = "gViewProj";
			constexpr char* Texture  = "gTexture";
		}
		namespace Standard3D
		{
			constexpr char* ViewProj = "gViewProj";
			constexpr char* Eye      = "gEye";
			constexpr char* World    = "gWorld";


			constexpr char* PointLightList = "gPointLightList";
			constexpr char* PointLightCount = "gPointLightCount";
		}

		namespace Type
		{
			constexpr char* Resources = "_Resources";
			constexpr char* Variables = "_Variables";
			constexpr char* Surface   = "_Surface";
		}
		namespace Variables
		{
			constexpr char* Surface = "__SurfaceConstantBuffer__";
		}

		namespace Location
		{
			constexpr char* SurfaceResources = "__PS_SURFACE_RESOURCES_SCRIPT__";
			constexpr char* SurfaceVariables = "__PS_SURFACE_VARIABLES_SCRIPT__";
			constexpr char* SurfaceContents  = "__PS_SURFACE_FUNCTION_SCRIPT__";
		}

		namespace Token
		{
			constexpr char* PassData = "__PASS_DATA__";
		}
	}
	namespace HLSL
	{
		using token = char;
		namespace Token
		{
			constexpr token* Struct = "struct ";
			constexpr token* CBuffer = "cbuffer ";
			constexpr token* StructuredBuffer = "StructuredBuffer";
			constexpr token* Texture2D = "Texture2D ";
			constexpr token* RWStructuredBuffer = "RWStructuredBuffer";
			constexpr token* RWTexture2D = "RWTexture2D ";
			constexpr token* SamplerState = "SamplerState ";
			constexpr token* SamplerComparisonState = "SamplerComparisonState ";

			namespace SamplerStateElement
			{
				constexpr token* Template = "Template";
				constexpr token* Min = "Min";
				constexpr token* Mag = "Mag";
				constexpr token* Mip = "Mip";
				constexpr token* AddressU = "AddressU";
				constexpr token* AddressV = "AddressV";
				constexpr token* AddressW = "AddressW";
				constexpr token* ComparisonFunc = "ComparisonFunc";
				constexpr token* BorderColor = "BorderColor";
				constexpr token* MinLOD = "MinLOD";
				constexpr token* MaxLOD = "MaxLOD";
				constexpr token* MaxAnisotropy = "MaxAnisotropy";
				constexpr token* MipLODBias = "MipLODBias";

			}
			namespace SamplerSatateFilter
			{
				constexpr token* Point = "Point";
				constexpr token* Linear = "Linear";
				constexpr token* Anisotropic = "Anisotropic";

			}
			namespace SamplerSatateAddressMode
			{
				constexpr token* Wrap = "Wrap";
				constexpr token* Mirror = "Mirror";
				constexpr token* Clamp = "Clamp";
				constexpr token* Border = "Border";
				constexpr token* MirrorOnce = "MirrorOnce";
			}
			namespace SamplerStateComparisonFunc
			{
				constexpr token* Never = "Never";
				constexpr token* Less = "Less";
				constexpr token* Equal = "Equal";
				constexpr token* LessEqual = "LessEqual";
				constexpr token* Greater = "Greater";
				constexpr token* NotEqual = "NotEqual";
				constexpr token* GreaterEqual = "GreaterEqual";
				constexpr token* Always = "Always";
			}
			namespace SamplerStateBorderColor
			{
				constexpr token* TransparentBlack = "TransparentBlack";
				constexpr token* OpaqueBlack = "OpaqueBlack";
				constexpr token* OpaqueWhite = "OpaqueWhite";
			}


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
			constexpr int RWTexture2DRegisterSpace = 2;
			constexpr int PointLightRegisterSpace = 3;
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
		constexpr char* CSTarget = "cs_5_1";





		enum class EHLSLElement
		{
			None,
			CBuffer,
			StructuredBuffer,
			RWStructuredBuffer,
			Texture,
			RWTexture,
			SamplerState,
		};

		enum class EHLSLTextureType
		{
			_1D,
			_2D,
			_3D,
			Cube,
		};
		inline String GetHLSLElementToken(EHLSLElement)
		{
			return "";
		}

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
				JG_CORE_CRITICAL("not supported ShaderDataType");
				return "unknown";
			}
		}



	}
}