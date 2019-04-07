#pragma once
#include"Common.h"
#include"Location.h"
#include"Scale.h"
#include"Rotation.h"


namespace Common
{
	class Transformation
	{
		Location m_Location;
		Rotation m_Rotation;
	    Scale    m_Scale;
		mutable JMatrix4x4 m_FinalMatrix;
		mutable bool       m_bUpdate = true;
	public:
		const Location& _Location_c() const {
			return m_Location;
		}
		Location& _Location() {
			m_bUpdate = true;
			return m_Location;
		}
		const Rotation& _Rotation_c() const {
			return m_Rotation;
		}
		Rotation& _Rotation() {
			m_bUpdate = true;
			return m_Rotation;
		}
		const Scale& _Scale_c() const {
			return m_Scale;
		}
		Scale& _Scale() {
			m_bUpdate = true;
			return m_Scale;
		}
		const JMatrix4x4& GetMatrix() const {
			UpdateMatrix();
			return m_FinalMatrix;
		}
		JMatrix4x4 GetHlslMatrix() const {
			UpdateMatrix();
			JMatrix4x4 result(m_FinalMatrix);
			result.Transpose();

			return result;
		}
	private:
		void UpdateMatrix() const
		{
			if (!m_bUpdate)
				return;
			m_FinalMatrix = m_Scale.GetMatrix() * m_Rotation.GetMatrix() * m_Location.GetMatrix();
			m_bUpdate = false;
		}
	};
}