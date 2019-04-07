#pragma once
namespace Dx12 {

	// º°Äª Á¤ÀÇ
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	namespace DxDevice {
		extern std::string g_LogGroupName;
	}



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

