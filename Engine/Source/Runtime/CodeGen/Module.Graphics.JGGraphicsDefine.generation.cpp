#include "PCH/PCH.h"
#include "Core.h"
#include "Runtime/Graphics/JGGraphicsDefine.h"


PSharedPtr<JGEnum> Module_Graphics_Code_Generation_Static_Create_Enum_ETextureFormat()
{

	HList<PSharedPtr<JGMeta>> MetaList;
	HList<PName> EnumStringList;
	HHashMap<int32, int32> EnumRedirectMap;

	MetaList.resize(16);
	EnumStringList.resize(16);

    EnumStringList[0] = "None";
    EnumRedirectMap[0] = 0;


	MetaList[0] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("0"),}),

	});

    EnumStringList[1] = "R8_Unorm";
    EnumRedirectMap[1] = 1;


	MetaList[1] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[2] = "R16_Float";
    EnumRedirectMap[2] = 2;


	MetaList[2] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[3] = "R32_Float";
    EnumRedirectMap[3] = 3;


	MetaList[3] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[4] = "R16G16_Float";
    EnumRedirectMap[4] = 4;


	MetaList[4] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("2"),}),

	});

    EnumStringList[5] = "R8G8B8A8_Unorm";
    EnumRedirectMap[5] = 5;


	MetaList[5] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[6] = "R16G16B16A16_Unorm";
    EnumRedirectMap[6] = 6;


	MetaList[6] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[7] = "R11G11B10_Float";
    EnumRedirectMap[7] = 7;


	MetaList[7] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("3"),}),

	});

    EnumStringList[8] = "R16G16B16A16_Float";
    EnumRedirectMap[8] = 8;


	MetaList[8] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[9] = "R32G32B32A32_Float";
    EnumRedirectMap[9] = 9;


	MetaList[9] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[10] = "R16G16B16A16_Uint";
    EnumRedirectMap[10] = 10;


	MetaList[10] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("4"),}),

	});

    EnumStringList[11] = "R8_Uint";
    EnumRedirectMap[11] = 11;


	MetaList[11] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[12] = "R16_Uint";
    EnumRedirectMap[12] = 12;


	MetaList[12] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[13] = "R32_Uint";
    EnumRedirectMap[13] = 13;


	MetaList[13] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("1"),}),

	});

    EnumStringList[14] = "R24G8_TYPELESS";
    EnumRedirectMap[14] = 14;


	MetaList[14] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("2"),}),

	});

    EnumStringList[15] = "D24_Unorm_S8_Uint";
    EnumRedirectMap[15] = 15;


	MetaList[15] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("Channels", {PName("2"),}),

	});

    return PObjectGlobalsPrivateUtils::MakeStaticEnum(JGType::GenerateType<ETextureFormat>(), "ETextureFormat", EnumRedirectMap, EnumStringList , MetaList);
}


PSharedPtr<JGEnum> Module_Graphics_Code_Generation_Static_Create_Enum_EResourceState()
{

	HList<PSharedPtr<JGMeta>> MetaList;
	HList<PName> EnumStringList;
	HHashMap<int32, int32> EnumRedirectMap;

	MetaList.resize(13);
	EnumStringList.resize(13);

    EnumStringList[0] = "Common";
    EnumRedirectMap[0] = 0;


	MetaList[0] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[1] = "VertexAndConstantBuffer";
    EnumRedirectMap[1] = 1;


	MetaList[1] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[2] = "IndexBuffer";
    EnumRedirectMap[2] = 2;


	MetaList[2] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[3] = "RenderTarget";
    EnumRedirectMap[3] = 3;


	MetaList[3] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[4] = "UnorderedAccess";
    EnumRedirectMap[4] = 4;


	MetaList[4] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[5] = "DepthWrite";
    EnumRedirectMap[5] = 5;


	MetaList[5] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[6] = "DepthRead";
    EnumRedirectMap[6] = 6;


	MetaList[6] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[7] = "NonePixelShaderResource";
    EnumRedirectMap[7] = 7;


	MetaList[7] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[8] = "PixelShaderResource";
    EnumRedirectMap[8] = 8;


	MetaList[8] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[9] = "CopyDest";
    EnumRedirectMap[9] = 9;


	MetaList[9] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[10] = "CopySource";
    EnumRedirectMap[10] = 10;


	MetaList[10] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[11] = "RayTracingAccelerationStructure";
    EnumRedirectMap[11] = 11;


	MetaList[11] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    EnumStringList[12] = "GenericRead";
    EnumRedirectMap[12] = 12;


	MetaList[12] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{

	});

    return PObjectGlobalsPrivateUtils::MakeStaticEnum(JGType::GenerateType<EResourceState>(), "EResourceState", EnumRedirectMap, EnumStringList , MetaList);
}


PSharedPtr<JGEnum> Module_Graphics_Code_Generation_Static_Create_Enum_ECommandQueueType()
{

	HList<PSharedPtr<JGMeta>> MetaList;
	HList<PName> EnumStringList;
	HHashMap<int32, int32> EnumRedirectMap;

	MetaList.resize(19);
	EnumStringList.resize(19);

    EnumStringList[0] = "Unknown";
    EnumRedirectMap[0] = 0;


	MetaList[0] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("Unknown"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("0"),}),

	});

    EnumStringList[1] = "Bool";
    EnumRedirectMap[1] = 1;


	MetaList[1] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("bool"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("1"),}),

	});

    EnumStringList[2] = "Int";
    EnumRedirectMap[2] = 2;


	MetaList[2] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("int"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("4"),}),

	});

    EnumStringList[3] = "Int2";
    EnumRedirectMap[3] = 3;


	MetaList[3] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("int2"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("8"),}),

	});

    EnumStringList[4] = "Int3";
    EnumRedirectMap[4] = 4;


	MetaList[4] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("int3"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("12"),}),

	});

    EnumStringList[5] = "Int4";
    EnumRedirectMap[5] = 5;


	MetaList[5] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("int4"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("16"),}),

	});

    EnumStringList[6] = "Uint";
    EnumRedirectMap[6] = 6;


	MetaList[6] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("uint"),}),
            HPair<PName, HHashSet<PName>>("			DataSize", {PName("4"),}),

	});

    EnumStringList[7] = "Uint2";
    EnumRedirectMap[7] = 7;


	MetaList[7] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("uint2"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("8"),}),

	});

    EnumStringList[8] = "Uint3";
    EnumRedirectMap[8] = 8;


	MetaList[8] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("uint3"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("12"),}),

	});

    EnumStringList[9] = "Uint4";
    EnumRedirectMap[9] = 9;


	MetaList[9] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("uint4"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("16"),}),

	});

    EnumStringList[10] = "Float";
    EnumRedirectMap[10] = 10;


	MetaList[10] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("4"),}),

	});

    EnumStringList[11] = "Float2";
    EnumRedirectMap[11] = 11;


	MetaList[11] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float2"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("8"),}),

	});

    EnumStringList[12] = "Float3";
    EnumRedirectMap[12] = 12;


	MetaList[12] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float3"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("12"),}),

	});

    EnumStringList[13] = "Float4";
    EnumRedirectMap[13] = 13;


	MetaList[13] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float4"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("16"),}),

	});

    EnumStringList[14] = "Float3x3";
    EnumRedirectMap[14] = 14;


	MetaList[14] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float3x3"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("36"),}),

	});

    EnumStringList[15] = "Float4x4";
    EnumRedirectMap[15] = 15;


	MetaList[15] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("float4x4"),}),
            HPair<PName, HHashSet<PName>>("		DataSize", {PName("64"),}),

	});

    EnumStringList[16] = "Texture2D";
    EnumRedirectMap[16] = 16;


	MetaList[16] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("	DataSize", {PName("0"),}),
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("Texture2D"),}),

	});

    EnumStringList[17] = "TextureCube";
    EnumRedirectMap[17] = 17;


	MetaList[17] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("	DataSize", {PName("0"),}),
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("TextureCube"),}),

	});

    EnumStringList[18] = "RWTexture2D";
    EnumRedirectMap[18] = 18;


	MetaList[18] = PObjectGlobalsPrivateUtils::MakeStaticMeta(
	{
            HPair<PName, HHashSet<PName>>("	DataSize", {PName("0"),}),
            HPair<PName, HHashSet<PName>>("HLSLName", {PName("RWTexture2D"),}),

	});

    return PObjectGlobalsPrivateUtils::MakeStaticEnum(JGType::GenerateType<ECommandQueueType>(), "ECommandQueueType", EnumRedirectMap, EnumStringList , MetaList);
}


