#include "pch.h"
#include "AnimationSkinning.h"
#include "Graphics/JGGraphics.h"
#include "Animation/AnimationTransform.h"
#include "Graphics/RootSignature.h"

namespace JG
{
	namespace Compute
	{
		AnimationSkinning::AnimationSkinning()
		{
			mShader = ShaderLibrary::GetInstance().FindComputeShader(SHADER_NAME);

			SharedPtr<IRootSignatureCreater> creater = IRootSignatureCreater::Create();
			creater->AddSRV(RootParam_InputVertices,  0, 0);
			creater->AddSRV(RootParam_InputBones,     1, 0);
			creater->AddDescriptorTable(RootParam_OutputVertices, EDescriptorTableRangeType::UAV, 1, 0, 0);
			creater->AddCBV(RootParam_CB, 0, 0);
			mRootSig = creater->Generate();

		}
		void AnimationSkinning::Execute(SharedPtr<IComputeContext> context, const Input& input)
		{
			if (input.AnimTransforms.empty() ||
				input.OriginMesh == nullptr || input.OriginMesh->IsValid() == false ||
				input.SkinnedMesh == nullptr || input.SkinnedMesh->IsValid() == false)
			{
				return;
			}
			if (mShader == nullptr || mShader->IsSuccessed() == false || mRootSig == nullptr)
			{
				return;
			}
			struct CB
			{
				AnimationTransform Transform;
				u32 VertexCount;
			};

			CB cb;

			context->BindShader(mShader);
			context->BindRootSignature(mRootSig);
			u32 subMeshCount = input.OriginMesh->GetSubMeshCount();
			for (u32 i = 0; i < subMeshCount; ++i)
			{
				SharedPtr<ISubMesh> subMesh = input.OriginMesh->GetSubMesh(i);
				SharedPtr<IVertexBuffer> vertexBuffer = subMesh->GetVertexBuffer();
				SharedPtr<IStructuredBuffer> boneBuffer = subMesh->GetBoneBuffer();

				if (boneBuffer == nullptr || boneBuffer->IsValid() == false) 
					continue;

				SharedPtr<ISubMesh> skinnedSubMesh = input.SkinnedMesh->GetSubMesh(i);

				if (skinnedSubMesh == nullptr || skinnedSubMesh->IsValid() == false) 
					continue;


				SharedPtr<IVertexBuffer> skinnedVertexBuffer = skinnedSubMesh->GetVertexBuffer();

				cb.VertexCount = subMesh->GetVertexBuffer()->GetVertexCount();
				cb.Transform   = *input.AnimTransforms[i];

				context->TransitionBarrier({ skinnedVertexBuffer }, { EResourceState::UnorderedAccess });
				context->BindConstantBuffer(RootParam_CB, cb);

				context->BindSturcturedBuffer(RootParam_InputVertices, vertexBuffer);
				context->BindSturcturedBuffer(RootParam_InputBones, boneBuffer);
				context->BindSturcturedBuffer(RootParam_OutputVertices, skinnedVertexBuffer);
				context->Dispatch1D(cb.VertexCount, 256);
				context->UAVBarrier({ skinnedVertexBuffer });
			}
		}
	}


}