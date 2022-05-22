#pragma once
#include "JGCore.h"


namespace JG
{
	class Skeletone;
	class SkeletalMask
	{
		Dictionary<String, f32> mSkeletalMaskDic;
	public:
		void AddMask(const String& boneName, f32 blendValueStep = 1.0f) {
			mSkeletalMaskDic[boneName] = blendValueStep;
		}
		void RemoveMask(const String& boneName) {
			mSkeletalMaskDic.erase(boneName);
		}
		bool IsExist(const String& boneName) const {
			return mSkeletalMaskDic.find(boneName) == mSkeletalMaskDic.end();
		}
		f32 GetBlendValueStep(const String& boneName) const {
			if (IsExist(boneName) == false)
			{
				return 0.0f;
			}
			return mSkeletalMaskDic.at(boneName);
		}
	};
}