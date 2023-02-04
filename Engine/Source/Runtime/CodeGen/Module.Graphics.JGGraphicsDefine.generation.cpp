#include "PCH/PCH.h"
#include "Core.h"
#include "Runtime/Graphics/JGGraphicsDefine.h"


PSharedPtr<JGEnum> Module_Graphics_Code_Generation_Static_Create_Enum_ETextureFormat()
{

	HList<PSharedPtr<JGMeta>> MetaList;
	HList<PName> EnumStringList;


	MetaList.resize(16);
	EnumStringList.resize(16);

    EnumStringList[0] = "None";


	MetaList[0] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("0"),}),

	});

    EnumStringList[1] = "R8_Unorm";


	MetaList[1] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[2] = "R16_Float";


	MetaList[2] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[3] = "R32_Float";


	MetaList[3] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[4] = "R16G16_Float";


	MetaList[4] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("2"),}),

	});

    EnumStringList[5] = "R8G8B8A8_Unorm";


	MetaList[5] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[6] = "R16G16B16A16_Unorm";


	MetaList[6] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[7] = "R11G11B10_Float";


	MetaList[7] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("3"),}),

	});

    EnumStringList[8] = "R16G16B16A16_Float";


	MetaList[8] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[9] = "R32G32B32A32_Float";


	MetaList[9] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[10] = "R16G16B16A16_Uint";


	MetaList[10] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[11] = "R8_Uint";


	MetaList[11] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[12] = "R16_Uint";


	MetaList[12] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[13] = "R32_Uint";


	MetaList[13] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[14] = "R24G8_TYPELESS";


	MetaList[14] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("2"),}),

	});

    EnumStringList[15] = "D24_Unorm_S8_Uint";


	MetaList[15] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("2"),}),

	});

    return PObjectGlobalsPrivateUtils::MakeStaticEnum(JGTYPE(ETextureFormat), "ETextureFormat", EnumStringList ,MetaList);
}


PSharedPtr<JGEnum> Module_Graphics_Code_Generation_Static_Create_Enum_EResourceState()
{

	HList<PSharedPtr<JGMeta>> MetaList;
	HList<PName> EnumStringList;


	MetaList.resize(13);
	EnumStringList.resize(13);

    EnumStringList[0] = "Common";


	MetaList[0] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[1] = "VertexAndConstantBuffer";


	MetaList[1] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[2] = "IndexBuffer";


	MetaList[2] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[3] = "RenderTarget";


	MetaList[3] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[4] = "UnorderedAccess";


	MetaList[4] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[5] = "DepthWrite";


	MetaList[5] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[6] = "DepthRead";


	MetaList[6] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[7] = "NonePixelShaderResource";


	MetaList[7] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[8] = "PixelShaderResource";


	MetaList[8] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[9] = "CopyDest";


	MetaList[9] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[10] = "CopySource";


	MetaList[10] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[11] = "RayTracingAccelerationStructure";


	MetaList[11] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[12] = "GenericRead";


	MetaList[12] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    return PObjectGlobalsPrivateUtils::MakeStaticEnum(JGTYPE(EResourceState), "EResourceState", EnumStringList ,MetaList);
}


PSharedPtr<JGEnum> Module_Graphics_Code_Generation_Static_Create_Enum_ECommandQueueType()
{

	HList<PSharedPtr<JGMeta>> MetaList;
	HList<PName> EnumStringList;


	MetaList.resize(19);
	EnumStringList.resize(19);

    EnumStringList[0] = "Unknown";


	MetaList[0] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("Unknown"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("0"),}),

	});

    EnumStringList[1] = "Bool";


	MetaList[1] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("bool"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("1"),}),

	});

    EnumStringList[2] = "Int";


	MetaList[2] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("int"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("4"),}),

	});

    EnumStringList[3] = "Int2";


	MetaList[3] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("int2"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("8"),}),

	});

    EnumStringList[4] = "Int3";


	MetaList[4] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("int3"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("12"),}),

	});

    EnumStringList[5] = "Int4";


	MetaList[5] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("int4"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("16"),}),

	});

    EnumStringList[6] = "Uint";


	MetaList[6] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("uint"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("4"),}),

	});

    EnumStringList[7] = "Uint2";


	MetaList[7] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("uint2"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("8"),}),

	});

    EnumStringList[8] = "Uint3";


	MetaList[8] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("uint3"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("12"),}),

	});

    EnumStringList[9] = "Uint4";


	MetaList[9] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("uint4"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("16"),}),

	});

    EnumStringList[10] = "Float";


	MetaList[10] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("4"),}),

	});

    EnumStringList[11] = "Float2";


	MetaList[11] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float2"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("8"),}),

	});

    EnumStringList[12] = "Float3";


	MetaList[12] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float3"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("12"),}),

	});

    EnumStringList[13] = "Float4";


	MetaList[13] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float4"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("16"),}),

	});

    EnumStringList[14] = "Float3x3";


	MetaList[14] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float3x3"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("36"),}),

	});

    EnumStringList[15] = "Float4x4";


	MetaList[15] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float4x4"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("64"),}),

	});

    EnumStringList[16] = "Texture2D";


	MetaList[16] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("	DataSize", {PName("0"),}),
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("Texture2D"),}),

	});

    EnumStringList[17] = "TextureCube";


	MetaList[17] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("	DataSize", {PName("0"),}),
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("TextureCube"),}),

	});

    EnumStringList[18] = "RWTexture2D";


	MetaList[18] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("	DataSize", {PName("0"),}),
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("RWTexture2D"),}),

	});

    return PObjectGlobalsPrivateUtils::MakeStaticEnum(JGTYPE(ECommandQueueType), "ECommandQueueType", EnumStringList ,MetaList);
}


