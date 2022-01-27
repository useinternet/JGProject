#pragma once
#include "JGCore.h"


namespace JG 
{
	class Renderer;
	class IComputeShader;
	class ITexture;
	class IComputeContext;
	enum class ETextureFormat;

	namespace Compute
	{
		class Blur_Float
		{
		public:
			struct Input
			{
				JVector2 Resolution;
				ETextureFormat TextureFormat;
				SharedPtr<ITexture> Src = nullptr;
			};
			struct Output
			{
				SharedPtr<ITexture> Result;
			};
		private:
	
			Renderer* mRenderer = nullptr;
			SharedPtr<IComputeShader> mShader;
		public:
			Blur_Float(Renderer* renderer);

			Output Execute(SharedPtr<IComputeContext> context, const Input& input);

		};
	}

}