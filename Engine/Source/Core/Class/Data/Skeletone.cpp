#include "pch.h"
#include "Skeletone.h"
#include "Class/Asset/Asset.h"


namespace JG
{



	const JMatrix& Skeletone::GetRootOffsetTransform() const
	{
		return mRootOffsetTransform;
	}

	u32 Skeletone::GetRootNodeID() const
	{
		return mRootBoneNode;
	}

	const Skeletone::Node* Skeletone::GetNode(u32 id) const
	{
		return &mBoneNodes[id];
	}

	bool Skeletone::IsValid() const
	{
		return mRootBoneNode != -1;
	}

	SharedPtr<Skeletone> Skeletone::Create(const SkeletalAssetStock& stock)
	{
		SharedPtr<Skeletone> skeletalone = CreateSharedPtr<Skeletone>();

		skeletalone->mName = stock.Name;
		skeletalone->mRootBoneNode = stock.RootBoneNode;
		skeletalone->mRootOffsetTransform = stock.RootOffset;
		for (const SkeletalAssetStock::BoneNode& boneNode : stock.BoneNodes)
		{
			Node node;
			node.ID = boneNode.ID;
			node.Name = boneNode.Name;

			node.ParentNode = boneNode.ParentNode;
			node.ChildNodes = boneNode.ChildNodes;

			node.Transform  = boneNode.Transform;
			node.BoneOffset = boneNode.BoneOffset;

			skeletalone->mBoneNodes.push_back(node);
		}
		return skeletalone;
	}


}