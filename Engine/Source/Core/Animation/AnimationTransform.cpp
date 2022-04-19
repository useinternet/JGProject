#include "pch.h"
#include "AnimationTransform.h"


namespace JG
{



	AnimationTransform::AnimationTransform()
	{
		Reset();
	}
	void AnimationTransform::Reset()
	{
		for (u32 i = 0; i < 64; ++i)
		{
			mTransform[i] = JMatrix::Identity();
		}
	}
	u32 AnimationTransform::GetMatrixCount() const
	{
		return 64;
	}
	const JMatrix* AnimationTransform::GetMatrixPointer() const
	{
		return mTransform;
	}
	void AnimationTransform::Set(i32 i, const JMatrix& m)
	{
		mTransform[i] = m;
	}
	const JMatrix& AnimationTransform::Get(i32 i) const
	{
		return mTransform[i];
	}
	JMatrix& AnimationTransform::operator[](i32 i)
	{
		return mTransform[i];
	}




}