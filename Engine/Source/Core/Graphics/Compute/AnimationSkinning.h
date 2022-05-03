#pragma once
#include "JGCore.h"


namespace JG
{

	class Renderer;
	class IComputeShader;
	class ITexture;
	class IMesh;
	class IComputeContext;
	class AnimationTransform;
	enum class ETextureFormat;
	namespace Compute
	{
		class AnimationSkinning
		{
		public:
			struct Input
			{
				SharedPtr<AnimationTransform> AnimTransform;
				SharedPtr<IMesh> OriginMesh;
				SharedPtr<IMesh> SkinnedMesh;
			};
		private:
			SharedPtr<IComputeShader> mShader;
		public:

			void Execute(SharedPtr<IComputeContext> context, const Input& input);

		};
	}


}