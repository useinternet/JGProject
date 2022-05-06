#include "Skeletone.h"
#include "Skeletone.h"
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

	void Skeletone::SetName(const String& name)
	{
		mName = name;
	}

	const String& Skeletone::GetName() const
	{
		return mName;
	}

	void Skeletone::SetSkeletalStock(const SkeletalAssetStock& stock)
	{
		mSkeletoneName = stock.Name;
		mRootBoneNode = stock.RootBoneNode;
		mRootOffsetTransform = stock.RootOffset;
		for (const SkeletalAssetStock::BoneNode& boneNode : stock.BoneNodes)
		{
			Node node;
			node.ID = boneNode.ID;
			node.Name = boneNode.Name;

			node.ParentNode = boneNode.ParentNode;
			node.ChildNodes = boneNode.ChildNodes;

			node.Transform  = boneNode.Transform;

			mBoneNodes.push_back(node);
		}
	}

	SharedPtr<Skeletone> Skeletone::Create(const String& name)
	{
		SharedPtr<Skeletone> skeletone = CreateSharedPtr<Skeletone>();
		skeletone->mName = name;
		return skeletone;
	}

	SharedPtr<Skeletone> Skeletone::Create(const SkeletalAssetStock& stock)
	{
		SharedPtr<Skeletone> skeletone = CreateSharedPtr<Skeletone>();
		skeletone->SetSkeletalStock(stock);
		return skeletone;
	}


}