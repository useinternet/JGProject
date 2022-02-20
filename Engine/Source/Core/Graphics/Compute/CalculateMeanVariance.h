#pragma once
#include "JGCore.h"


namespace JG
{
	class IComputeContext;
	class IRootSignature;
	class ITexture;
	class Renderer;
	class IComputeShader;
	class CalculateMeanVariance
	{
	public:
		struct Input
		{
			JVector2 Resolution;
			SharedPtr<ITexture> Value;
			SharedPtr<ITexture> OutMeanVariance;
			u32 KernelWidth; //9, 3, 9
		};
	private:
		Renderer* mRenderer;
		SharedPtr<IComputeShader> mShader;


	public:
		CalculateMeanVariance(Renderer* renderer);
		void Execute(SharedPtr<IComputeContext> context, const Input& input);

	};
}