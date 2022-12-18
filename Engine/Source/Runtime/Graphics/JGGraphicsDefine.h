#pragma once


#ifdef _GRAPHICS
#define GRAPHICS_API __declspec(dllexport)
#define GRAPHICS_C_API extern "C" __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#define GRAPHICS_C_API extern "C" __declspec(dllimport)
#endif

#define MAX_RENDERTARGET 8
#define TEXTURE_ID_NULL  0

using TextureID					= uint64;
using PrimitiveResourcePtr		= void*;
using ResourceGPUVirtualAddress = uint64;
using ResourceViewPtr			= uint64;

JGENUM()
enum class ETextureFormat
{
	None,					JGENUMMETA(Channels = 0)
	R8_Unorm,				JGENUMMETA(Channels = 1)
	R16_Float,				JGENUMMETA(Channels = 1)
	R32_Float,				JGENUMMETA(Channels = 1)
	R16G16_Float,			JGENUMMETA(Channels = 2)
	R8G8B8A8_Unorm,			JGENUMMETA(Channels = 4)
	R16G16B16A16_Unorm,		JGENUMMETA(Channels = 4)
	R11G11B10_Float,		JGENUMMETA(Channels = 3)
	R16G16B16A16_Float,		JGENUMMETA(Channels = 4)
	R32G32B32A32_Float,		JGENUMMETA(Channels = 4)
	R16G16B16A16_Uint,		JGENUMMETA(Channels = 4)
	R8_Uint,				JGENUMMETA(Channels = 1)
	R16_Uint,				JGENUMMETA(Channels = 1)
	R32_Uint,				JGENUMMETA(Channels = 1)
	R24G8_TYPELESS,			JGENUMMETA(Channels = 2)
	D24_Unorm_S8_Uint,		JGENUMMETA(Channels = 2)
};

JGENUM()
enum class EResourceState
{
	Common,								JGENUMMETA()
	VertexAndConstantBuffer,			JGENUMMETA()
	IndexBuffer,						JGENUMMETA()
	RenderTarget,						JGENUMMETA()
	UnorderedAccess,					JGENUMMETA()
	DepthWrite,							JGENUMMETA()
	DepthRead,							JGENUMMETA()
	NonePixelShaderResource,			JGENUMMETA()
	PixelShaderResource,				JGENUMMETA()
	CopyDest,							JGENUMMETA()
	CopySource,							JGENUMMETA()
	RayTracingAccelerationStructure,	JGENUMMETA()
	GenericRead,						JGENUMMETA()
};

JGENUM()
enum class EShaderDataType
{
	Unknown,			JGENUMMETA(HLSLName = Unknown,		DataSize = 0)
	Bool,				JGENUMMETA(HLSLName = bool,			DataSize = 1)
	Int,				JGENUMMETA(HLSLName = int,			DataSize = 4)
	Int2,				JGENUMMETA(HLSLName = int2,			DataSize = 8)
	Int3,				JGENUMMETA(HLSLName = int3,			DataSize = 12)
	Int4,				JGENUMMETA(HLSLName = int4,			DataSize = 16)
	Uint,				JGENUMMETA(HLSLName = uint,			DataSize = 4)
	Uint2,				JGENUMMETA(HLSLName = uint2,		DataSize = 8)
	Uint3,				JGENUMMETA(HLSLName = uint3,		DataSize = 12)
	Uint4,				JGENUMMETA(HLSLName = uint4,		DataSize = 16)
	Float,				JGENUMMETA(HLSLName = float,		DataSize = 4)
	Float2,				JGENUMMETA(HLSLName = float2,		DataSize = 8)
	Float3,				JGENUMMETA(HLSLName = float3,		DataSize = 12)
	Float4,				JGENUMMETA(HLSLName = float4,		DataSize = 16)
	Float3x3,			JGENUMMETA(HLSLName = float3x3,		DataSize = 36)
	Float4x4,			JGENUMMETA(HLSLName = float4x4,		DataSize = 64)
	Texture2D,			JGENUMMETA(HLSLName = Texture2D,	DataSize = 0)
	TextureCube,		JGENUMMETA(HLSLName = TextureCube,	DataSize = 0)
	RWTexture2D,		JGENUMMETA(HLSLName = RWTexture2D,	DataSize = 0)
};

enum class EDepthStencilStateTemplate
{
	Default = 0,
	NoDepth = 1,
	LessEqual = 2,
};

enum class EBlendStateTemplate
{
	Default = 0,
	Transparent_Default,
};

enum class ERasterizerStateTemplate
{
	Default = 0,
	Cull_None = 1,
};

JG_ENUM_FLAG(ETextureFlags)
enum class ETextureFlags
{
	None = 0x00001,
	Allow_RenderTarget = 0x00002,
	Allow_DepthStencil = 0x00004,
	Allow_UnorderedAccessView = 0x00008,
	SRV_TextureCube = 0x00010,
};

JG_ENUM_FLAG(EShaderFlags)
enum class EShaderFlags
{
	None = 0x000,
	Allow_VertexShader = 0x001,
	Allow_PixelShader = 0x002,
	Allow_GeometryShader = 0x004,
	Allow_HullShader = 0x008,
	Allow_DomainShader = 0x010,
	Allow_ComputeShader = 0x020,
};


class HInputElement
{
public:
	const char* SementicName = nullptr;
	uint32 SementicSlot = 0;
	EShaderDataType Type = EShaderDataType::Unknown;
public:
	HInputElement(EShaderDataType type, const char* sementicName, uint32 sementicSlot)
		: Type(type), SementicName(sementicName), SementicSlot(sementicSlot) {}
};

class HInputLayout
{
private:
	HList<HInputElement> _inputElements;

public:
	void Add(EShaderDataType type, const char* sementicName, uint32 sementicSlot)
	{
		_inputElements.push_back(HInputElement(type, sementicName, sementicSlot));
	}

	void ForEach(const std::function<void(const HInputElement&)>& action) const
	{
		for (auto& element : _inputElements)
		{
			action(element);
		}
	}
};

struct HTextureInfo
{
	PString Name;
	uint32 Width = 0;
	uint32 Height = 0;
	uint32 PixelPerUnit = 1;
	ETextureFormat Format = ETextureFormat::None;
	ETextureFlags  Flags  = ETextureFlags::None;
	uint16 MipLevel = 0;
	uint16 ArraySize = 1;

	HColor   ClearColor;
	float32  ClearDepth = 1.0f;
	uint8    ClearStencil = 0;

	HTextureInfo()
	{
		Name = "Texture";
	}
};

struct MeshInfo
{
	PName Name;
	uint32 TotalSubMeshCount = 0;
	uint32 TotalVertexCount = 0;
	uint32 TotalIndexCount = 0;

	HList<PName> SubMeshNames;
	HList<uint32>    SubMeshVertexCounts;
	HList<uint32>    SubMeshIndexCounts;

	HInputLayout InputLayout;
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
	Unknown,
	Graphics,
	Compute
};

//namespace ShaderDefine
//{
//	namespace Template
//	{
//		constexpr char* Standard2DShader = "Standard2DShader";
//		constexpr char* Standard3DShader = "Standard3DShader";
//		constexpr char* StandardSceneShader = "StandardSceneShader";
//		constexpr char* StandardEditorUIShader = "StandardEditorUIShader";
//		constexpr char* DebugShader = "DebugShader";
//		constexpr char* DirectClosestHitShader = "DirectClosestHitShader";
//		constexpr char* IndirectClosestHitShader = "IndirectClosestHitShader";
//	}
//	namespace Type
//	{
//		constexpr char* Resources = "_Resources";
//		constexpr char* Function = "_Function";
//		constexpr char* Variables = "_Variables";
//		constexpr char* Surface = "_Surface";
//		constexpr char* Scene = "_Scene";
//	}
//
//	namespace Location
//	{
//		constexpr char* SurfaceResources = "__PS_SURFACE_RESOURCES_SCRIPT__";
//		constexpr char* SurfaceVariables = "__PS_SURFACE_VARIABLES_SCRIPT__";
//		constexpr char* SurfaceFunction = "__PS_SURFACE_FUNCTION_SCRIPT__";
//		constexpr char* SurfaceContents = "__PS_SURFACE_CONTENTS_SCRIPT__";
//
//
//		constexpr char* SceneResources = "__PS_SCENE_RESOURCES_SCRIPT__";
//		constexpr char* SceneVariables = "__PS_SCENE_VARIABLES_SCRIPT__";
//		constexpr char* SceneFunction = "__PS_SCENE_FUNCTION_SCRIPT__";
//		constexpr char* SceneContents = "__PS_SCENE_CONTENTS_SCRIPT__";
//	}
//
//	namespace RayTracing
//	{
//		constexpr char* RayGen = "RayGeneration";
//		constexpr char* DirectNullHit = "DirectClosestHit";
//		constexpr char* DirectMiss = "DirectMiss";
//
//
//		constexpr char* IndirectNullHit = "IndirectClosestHit";
//		constexpr char* IndirectMiss = "IndirectMiss";
//
//		constexpr char* ShadowHit = "ShadowHit";
//		constexpr char* ShadowMiss = "ShadowMiss";
//
//		constexpr char* DirectNullHitGroup = "DirectNullHitGroup";
//		constexpr char* IndirectNullHitGroup = "IndirectNullHitGroup";
//		constexpr char* ShadowHitGroup = "ShadowHitGroup";
//	}
//}
//namespace HLSL
//{
//	using token = char;
//	namespace Token
//	{
//		constexpr token* Struct = "struct ";
//		constexpr token* CBuffer = "cbuffer ";
//		constexpr token* StructuredBuffer = "StructuredBuffer";
//		constexpr token* Texture2D = "Texture2D";
//		constexpr token* TextureCube = "TextureCube";
//		constexpr token* RWStructuredBuffer = "RWStructuredBuffer";
//		constexpr token* RWTexture2D = "RWTexture2D<";
//		constexpr token* ByteAddressBuffer = "ByteAddressBuffer";
//		constexpr token* RWByteAddressBuffer = "RWByteAddressBuffer";
//		constexpr token* SamplerState = "SamplerState";
//		constexpr token* SamplerComparisonState = "SamplerComparisonState";
//	}
//	namespace RegisterNumber
//	{
//		constexpr int RenderPassRegisterNumber = 0;
//		constexpr int ObjectRegisterNumber = 1;
//		constexpr int MaterialRegisterNumber = 2;
//	}
//	namespace RegisterSpace
//	{
//		constexpr int Texture2DRegisterSpace = 0;
//		constexpr int TextureCubeRegisterSpace = 1;
//	}
//
//	constexpr char* VSEntry = "vs_main";
//	constexpr char* DSEntry = "ds_main";
//	constexpr char* HSEntry = "hs_main";
//	constexpr char* GSEntry = "gs_main";
//	constexpr char* PSEntry = "ps_main";
//	constexpr char* CSEntry = "cs_main";
//	constexpr char* VSTarget = "vs_5_1";
//	constexpr char* DSTarget = "ds_5_1";
//	constexpr char* HSTarget = "hs_5_1";
//	constexpr char* GSTarget = "gs_5_1";
//	constexpr char* PSTarget = "ps_5_1";
//	constexpr char* CSTarget = "cs_6_0";
//	constexpr char* RTTarget = "lib_6_3";
//	constexpr char* ClosestHitEntry = "__ClosestHit_EntryPoint__";
//	constexpr char* MissEntry = "__Miss_EntryPoint__";
//
//
//	inline String ShaderDataTypeToHLSLCode(EShaderDataType type)
//	{
//		switch (type)
//		{
//		case EShaderDataType::_bool:	return "bool";
//		case EShaderDataType::_int:		return "int";
//		case EShaderDataType::_int2:	return "int2";
//		case EShaderDataType::_int3:	return "int3";
//		case EShaderDataType::_int4:	return "int4";
//		case EShaderDataType::_uint:	return "uint";
//		case EShaderDataType::_uint2:	return "uint2";
//		case EShaderDataType::_uint3:	return "uint3";
//		case EShaderDataType::_uint4:	return "uin4";
//		case EShaderDataType::_float:	return "float";
//		case EShaderDataType::_float2:	return "float2";
//		case EShaderDataType::_float3:	return "float3";
//		case EShaderDataType::_float4:   return "float4";
//		case EShaderDataType::_float3x3: return "float3x3";
//		case EShaderDataType::_float4x4: return "float4x4";
//		default:
//			JG_LOG_ERROR("not supported ShaderDataType");
//			return "unknown";
//		}
//	}
//}