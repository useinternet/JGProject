#pragma once

#include "JGCore.h"


namespace JG
{
	class IComputeContext;
	class IRootSignature;
	class ITexture;
	class Renderer;
	class IComputeShader;
	class DisocclusionBilateralFilter
	{
	public:
		struct Input
		{
			JVector2 Resolution;
			u32 Step;

			SharedPtr<ITexture> Depth;
			SharedPtr<ITexture> BlurStrength;
			SharedPtr<ITexture> OutValue;
		};

	private:
		Renderer* mRenderer;
		SharedPtr<IComputeShader> mShader;


	public:
		DisocclusionBilateralFilter(Renderer* renderer);
		void Execute(SharedPtr<IComputeContext> context, const Input& input);


	};
}