#pragma once
#include "JGCore.h"

namespace JG
{
	class AnimationTransform
	{
		JMatrix mTransform[64];
	public:
		AnimationTransform();

		void Reset();
		u32 GetMatrixCount() const;
		const JMatrix* GetMatrixPointer() const;

		void Set(i32 i, const JMatrix& m);
		const JMatrix& Get(i32 i) const;
	public:
		JMatrix& operator[](i32 i);
	};
}