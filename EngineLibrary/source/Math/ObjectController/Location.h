#pragma once
#include"Math/GeoMath.h"
#include"Math/Transform.h"

namespace JGLibrary
{
	class Location
	{
	private:
		JPoint3   m_Location;
		Transform m_Transform;
		bool      m_bUpdate = true;
	public:
		Location()  = default;
		~Location() = default;
		Location(float x, float y, float z) : m_Location(x, y, z) {}
		Location(const JPoint3& p)  : m_Location(p) {}
		Location(const JVector3& v) : m_Location(v) {}
	public:
		Location& operator=(const Location& rhs) {
			m_Location  = rhs.m_Location;
			m_Transform = rhs.m_Transform;
		}
		bool IsUpdate() {
			return m_bUpdate;
		}
		void Set(float x, float y, float z) {
			m_bUpdate = true;
			m_Location = JPoint3(x,y,z);
		}
		void Set(const JPoint3& p) {
			m_bUpdate = true;
			m_Location = p;
		}
		void Set(const JVector3& v) {
			m_bUpdate = true;
			m_Location = JPoint3(v);
		}
		void SetX(float x) {
			m_bUpdate = true;
			m_Location.mData.x = x;
		}
		void SetY(float y) {
			m_bUpdate = true;
			m_Location.mData.y = y;
		}
		void SetZ(float z) {
			m_bUpdate = true;
			m_Location.mData.z = z;
		}
		void Offset(float x, float y, float z) {
			m_bUpdate = true;
			m_Location += JPoint3(x, y, z);
		}
		void Offset(const JPoint3& p) {
			m_bUpdate = true;
			m_Location += p;
		}
		void Offset(const JVector3& v) {
			m_bUpdate = true;
			m_Location += v;
		}
		void OffsetX(float x){
			m_bUpdate = true;
			m_Location.mData.x += x;
		}
		void OffsetY(float y) {
			m_bUpdate = true;
			m_Location.mData.y += y;
		}
		void OffsetZ(float z) {
			m_bUpdate = true;
			m_Location.mData.z += z;
		}
		const JPoint3& Get_c() const {
			return m_Location;
		}
		JPoint3& Get() {
			return m_Location;
		}
		Transform& GetTransform() {
			if (m_bUpdate)
			{
				m_Transform.Identity();
				m_Transform.Translation(m_Location);
				m_bUpdate = false;
			}
			return m_Transform;
		}
		float X() const {
			return m_Location.mData.x;
		}
		float Y() const {
			return m_Location.mData.y;
		}
		float Z() const {
			return m_Location.mData.z;
		}
		const xmfloat3& Data_3f() const{
			return m_Location.mData;
		}
		const xmfloat4x4& Data_4x4f() {
			if (m_bUpdate)
			{
				m_Transform.Identity();
				m_Transform.Translation(m_Location);
				m_bUpdate = false;
			}
			return m_Transform.mData;
		}
	};
}


