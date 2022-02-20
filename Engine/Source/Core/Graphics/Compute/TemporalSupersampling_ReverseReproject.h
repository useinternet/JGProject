#pragma once
#include "JGCore.h"


namespace JG
{
	class IComputeContext;
	class IRootSignature;
	class ITexture;
	class Renderer;
	class IComputeShader;
	class TemporalSupersampling_ReverseReproject
	{
	public:
		struct Input
		{
			JVector2 Resolution;
			f32 DepthSigma;

			SharedPtr<ITexture> CurrentFrameNormalDepth;
			SharedPtr<ITexture> CurrentFrameDepthDerivative;
			SharedPtr<ITexture> ReprojectedNormalDepth;
			SharedPtr<ITexture> MotionVector;
			SharedPtr<ITexture> CachedNormalDepth;
			SharedPtr<ITexture> CachedValue;
			SharedPtr<ITexture> CachedTspp;
			SharedPtr<ITexture> CachedValueSquareMean;
			SharedPtr<ITexture> CachedRayHitDepth;

			SharedPtr<ITexture> OutCachedTspp;
			SharedPtr<ITexture> OutReprojectedCachedValues;
			SharedPtr<ITexture> OutDebug;
		};
	private:
		Renderer* mRenderer;
		SharedPtr<IComputeShader> mShader;

	
	public:
		TemporalSupersampling_ReverseReproject(Renderer* renderer);
		void Execute(SharedPtr<IComputeContext> context, const Input& input);

	};
}