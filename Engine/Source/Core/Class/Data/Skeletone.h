#pragma once
#include "Common/Define.h"
#include "Math/JMatrix.h"


namespace JG
{
	class SkeletalAssetStock;
	class Skeletone
	{
	public:
		struct Node
		{
			u32 ID;
			String Name;

			u32 ParentNode;
			List<u32> ChildNodes;

			JMatrix Transform;
			JMatrix BoneOffset;
		};
	private:
		String mName;
		String mSkeletoneName;
		u32	   mRootBoneNode = -1;
		List<Node> mBoneNodes;
		JMatrix    mRootOffsetTransform;
	public:
		const JMatrix& GetRootOffsetTransform() const;
		u32 GetRootNodeID() const;

		const Skeletone::Node* GetNode(u32 id) const;

		bool IsValid() const;


		void SetName(const String& name);
		const String& GetName() const;

		void SetSkeletalStock(const SkeletalAssetStock& stock);

		static SharedPtr<Skeletone> Create(const String& name);
		static SharedPtr<Skeletone> Create(const SkeletalAssetStock& stock);
	};
}
