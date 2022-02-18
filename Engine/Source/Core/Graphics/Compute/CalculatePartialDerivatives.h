#pragma once
#include "JGCore.h"


namespace JG
{
	class IRootSignature;
	class IComputeContext;
	class Renderer;
	class ITexture;
	class IComputeShader;
	class CalculatePartialDerivatives
	{
	public:
		struct Input
		{
			JVector2 Resolution;
			SharedPtr<ITexture> Input;
			SharedPtr<ITexture> Output;
		};
	private:
		Renderer* mRenderer = nullptr;
		SharedPtr<IComputeShader> mShader;
	public:
		CalculatePartialDerivatives() = default;
		CalculatePartialDerivatives(Renderer* renderer);
		void Execute(SharedPtr<IComputeContext> context, const Input& input);
	};


}