#pragma once
#include"DirectXCommon/DirectXCommon.h"
#include"RenderTarget.h"
#include"Texture.h"
#include"Camera.h"



namespace Dx12
{
	typedef int ECubeFace;
	namespace CubeFace
	{
		enum
		{
			Right = 0,
			Left,
			Up,
			Down,
			Forward,
			Back,
			Num_Face
		};
	}
	class CubeRenderTarget
	{
	private:
		RenderTarget m_RenderTargets[CubeFace::Num_Face];
		Camera       m_Camera[CubeFace::Num_Face];
		std::vector<Texture> m_Textures;

	public:

	};
}