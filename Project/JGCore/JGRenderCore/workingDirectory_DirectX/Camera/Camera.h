#pragma once
#include"Common/JGRCCommon.h"
#include"Common/JGRCGlobalType.h"
#include"DirectXCommon.h"

namespace JGRC
{
	class CORE_EXPORT Camera
	{
	private:
		jgVec3 m_Eye;
		jgVec3 m_Up;
		jgVec3 m_LookAt;
		std::unique_ptr<jgMatrix4x4> m_viewMatrix;
	public:
		Camera();
		jgVec3* GetEyeData() { return &m_Eye; }
		void SetEye(const real x, const real y, const real z);
		const jgMatrix4x4& GetViewMatrix() const;
	private:
		void CreateViewMatrix();
	};
}
