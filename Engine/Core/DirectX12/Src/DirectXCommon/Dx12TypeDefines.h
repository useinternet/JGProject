#pragma once
namespace Dx12 {

	// 별칭 정의
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	namespace DxDevice {
		extern std::string g_LogGroupName;
	}

	// Enum | 연산
	//template<typename EnumType>
	//EnumType operator|(EnumType e1, EnumType e2)
	//{
	//	return (int)e1 | (int)e2;
	//}

	// 텍스쳐 타입
	typedef int ETextureUsage;
	namespace TextureUsage
	{
		enum
		{
			Diffuse = 0,
			Albedo = Diffuse,
			Normal,
			RenderTarget,
			DepthStencil,
			UnorderedAccess,
		};
	}

	// 텍스쳐 슬롯
	typedef int ETextureSlot;
	namespace TextureSlot
	{
		enum
		{
			Slot0 = 0,
			Slot1,
			Slot2,
			Slot3,
			Slot4,
			Slot5,
			Slot6,
			Slot7,
			NumSlot,
			Diffuse = Slot0,
			Normal  = Slot1
		};
	}

	// 큐브 텍스쳐 슬롯
	typedef int ECubeTextureSlot;
	namespace CubeTextureSlot
	{
		enum
		{
			Slot0 = 0,
			Slot1,
			Slot2,
			Slot3,
			NumSlot,
			Reflection = Slot0
		};
	}



	// 셰이더 단계
	typedef int EShaderStage;
	namespace ShaderStage
	{
		enum
		{
			Pixel,
			Vertex,
			Hull,
			Domain,
			Geometry,
			Compute,
		};
	}

	// HLSL 타입
	typedef int EHLSLVersion;
	namespace HLSLVersion
	{
		enum
		{
			_4_0,
			_5_0,
			_5_1
		};
	}

}

