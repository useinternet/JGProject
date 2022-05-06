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
	class IRootSignature;
	class IStructuredBuffer;
	enum class ETextureFormat;
	namespace Compute
	{
		class AnimationSkinning
		{
			static constexpr char* SHADER_NAME = "AnimationSkinning";
		public:
			enum
			{
				RootParam_InputVertices,
				RootParam_InputBones,
				RootParam_OutputVertices,
				RootParam_CB,
			};
			struct Input
			{
				List<SharedPtr<AnimationTransform>> AnimTransforms;
				SharedPtr<IMesh> OriginMesh;
				SharedPtr<IMesh> SkinnedMesh;
				SharedPtr<IStructuredBuffer> Test;
			};
		private:
			SharedPtr<IRootSignature> mRootSig;
			SharedPtr<IComputeShader> mShader;
		public:
			AnimationSkinning();
			void Execute(SharedPtr<IComputeContext> context, const Input& input);

		};
	}


}