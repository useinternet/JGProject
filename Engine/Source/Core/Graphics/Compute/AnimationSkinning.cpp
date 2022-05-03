#include "pch.h"
#include "AnimationSkinning.h"
#include "Graphics/JGGraphics.h"

namespace JG
{
	namespace Compute
	{
		void AnimationSkinning::Execute(SharedPtr<IComputeContext> context, const Input& input)
		{
			if (input.AnimTransform == nullptr ||
				input.OriginMesh == nullptr || input.OriginMesh->IsValid() == false ||
				input.SkinnedMesh == nullptr || input.SkinnedMesh->IsValid() == false)
			{

			}
		}
	}


}