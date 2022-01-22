#pragma once
#include "JGCore.h"

namespace JG
{
	class IComputeShader;
	class IComputeContext;
	class ITexture;
	class Renderer;
	namespace Compute
	{
		class FloatAccumulater
		{
		public:
			struct Input
			{
				JVector2 Resolution;
				bool IsResetAccumCount = false;
				SharedPtr<ITexture> CurrFrame = nullptr;
			};
			struct Output
			{
				SharedPtr<ITexture> Result;
			};
		private:
			struct CB
			{
				u32 AccumCount = 0;
			};


			Renderer* mRenderer = nullptr;
			SharedPtr<IComputeShader> mShader;
			List<SharedPtr<ITexture>> mPrevs;
			List<SharedPtr<ITexture>> mResults;
			CB mCB;
			JVector2 mResolution;
		public:
			FloatAccumulater(Renderer* renderer);

			Output Execute(SharedPtr<IComputeContext> context, const Input& input);
			void ResetAccumCount();
		private:
			void InitTextures(const JVector2& resolution);
			void Swap(SharedPtr<ITexture>& t1, SharedPtr<ITexture>& t2);
		};
	}




}