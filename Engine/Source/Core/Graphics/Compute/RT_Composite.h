#pragma once
#include "JGCore.h"


namespace JG
{
	class IRootSignature;
	class IComputeContext;
	class Renderer;
	class ITexture;
	class IComputeShader;
	class RT_Composite
	{
	public:
		struct Input
		{
			JVector2 Resolution;
			f32 FarZ;
			SharedPtr<ITexture> Direct;
			SharedPtr<ITexture> IndirectR;
			SharedPtr<ITexture> IndirectG;
			SharedPtr<ITexture> IndirectB;
			SharedPtr<ITexture> Emissive;
			SharedPtr<ITexture> Shadow;
			SharedPtr<ITexture> AO;
			SharedPtr<ITexture> Depth;
			SharedPtr<ITexture> Output;
		};
		
	private:
		Renderer* mRenderer = nullptr;
		SharedPtr<IComputeShader> mShader;
	public:
		RT_Composite() = default;
		RT_Composite(Renderer* renderer);
		void Execute(SharedPtr<IComputeContext> context, const Input& input);
	};


}