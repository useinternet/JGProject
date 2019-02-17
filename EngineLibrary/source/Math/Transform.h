#pragma once
#include"GeoMath.h"

namespace JGLibrary
{
	class Quaternion;
	class Transform
	{
	public:
		xmfloat4x4 mData;
	public:
		Transform() {
			Identity();
		}
		Transform(const xmfloat4x4& m)
			: mData(m) {}
		Transform(FSimMatrix m) {
			SetSIMD(m);
		}
	public:
		float Determinant() const {
			return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(GetSIMD()));
		}
		void Inverse() {
			SimVector SimDet = DirectX::XMMatrixDeterminant(GetSIMD());
			SetSIMD(DirectX::XMMatrixInverse(&SimDet, GetSIMD()));
		}
		void Inverse(Transform* pOut) const {
			SimVector SimDet = DirectX::XMMatrixDeterminant(GetSIMD());
			pOut->SetSIMD(DirectX::XMMatrixInverse(&SimDet, GetSIMD()));
		}
		void Transpose() {
			SimMatrix m = DirectX::XMMatrixTranspose(GetSIMD());
			SetSIMD(m);
		}
		void Transpose(Transform* pOut) const {
			SimMatrix m = DirectX::XMMatrixTranspose(GetSIMD());
			pOut->SetSIMD(m);
		}
		void Translation(const JPoint3& p) {
			SimMatrix m = DirectX::XMMatrixTranslation(p.mData.x, p.mData.y, p.mData.z);
			SetSIMD(m);
		}
		void RotationX(float angle) {
			SimMatrix m = DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(angle));
			SetSIMD(m);
		}
		void RotationY(float angle) {
			SimMatrix m = DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(angle));
			SetSIMD(m);
		}
		void RotationZ(float angle) {
			SimMatrix m = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(angle));
			SetSIMD(m);
		}
		void Rotation(const JPoint3& p) {
			SimMatrix m = DirectX::XMMatrixRotationRollPitchYaw(
				DirectX::XMConvertToRadians(p.mData.x), 
				DirectX::XMConvertToRadians(p.mData.y),
				DirectX::XMConvertToRadians(p.mData.z));
			SetSIMD(m);
		}
		void Scaling(const JPoint3& p) {
			SimMatrix m = DirectX::XMMatrixScaling(p.mData.x, p.mData.y, p.mData.z);
			SetSIMD(m);
		}
		void RotationAxis(const JVector3& axis, float angle) {
			SimMatrix m = DirectX::XMMatrixRotationAxis(
				axis.GetSIMD(), 
				DirectX::XMConvertToRadians(angle));
			SetSIMD(m);
		}
		void RotationNormalAxis(const JNormal3& axis, float angle) {
			SimMatrix m = DirectX::XMMatrixRotationNormal(
				axis.GetSIMD(),
				DirectX::XMConvertToRadians(angle));
			SetSIMD(m);
		}
		void Identity() {
			SimMatrix m = DirectX::XMMatrixIdentity();
			SetSIMD(m);
		}
		void LookAt(const JPoint3& eyepos, const JPoint3& targetpos, const JVector3& updir) {
			SimMatrix m = DirectX::XMMatrixLookAtLH(eyepos.GetSIMD(), targetpos.GetSIMD(), updir.GetSIMD());
			SetSIMD(m);
		}
		void LookTo(const JPoint3& eyepos, const JVector3& eyedir, const JVector3& updir) {
			SimMatrix m = DirectX::XMMatrixLookToLH(eyepos.GetSIMD(), eyedir.GetSIMD(), updir.GetSIMD());
			SetSIMD(m);
		}
		void Perspective(float fov, float aspect_ratio, float nearZ, float farZ) {
			SimMatrix m = DirectX::XMMatrixPerspectiveFovLH(fov, aspect_ratio, nearZ, farZ);
			SetSIMD(m);
		}
		void Orthographic(float width, float height, float nearZ, float farZ) {
			SimMatrix m= DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ);
			SetSIMD(m);
		}
		void DeCompose(JPoint3* pos, Quaternion* quat, JPoint3* scale);
	public:
		bool IsIdentity() const {
			return DirectX::XMMatrixIsIdentity(GetSIMD());
		}
		bool IsSwapHandedness() const{
			float det = Determinant();
			return det < 0.0f;
		}
	public:
		Transform operator*(const Transform& t) const {
			return Transform(DirectX::XMMatrixMultiply(GetSIMD(), t.GetSIMD()));
		}
	public:
		JPoint3 operator()(const JPoint3& p) const {
			return JPoint3(DirectX::XMVector3TransformCoord(p.GetSIMD(), GetSIMD()));
		}
		void operator()(const JPoint3& p, JPoint3* pOut) const {
			pOut->SetSIMD(DirectX::XMVector3TransformCoord(p.GetSIMD(), GetSIMD()));
		}
		JVector3 operator()(const JVector3& v) const {
			return JVector3(DirectX::XMVector3TransformNormal(v.GetSIMD(), GetSIMD()));
		}
		void operator()(const JVector3& v, JVector3* pOut) const {
			pOut->SetSIMD(DirectX::XMVector3TransformNormal(v.GetSIMD(), GetSIMD()));
		}
		// º¸·ù
		//JNormal3 operator()(const JNormal3& n) const {
		//	return JNormal3(DirectX::XMVector3TransformNormal(n.GetSIMD(), GetSIMD()));
		//}
		//void operator()(const JNormal3& n, JNormal3* pOut) const {
		//	pOut->SetSIMD(DirectX::XMVector3TransformNormal(n.GetSIMD(), GetSIMD()));
		//}
		JRay operator()(const JRay& r) const {
			JRay ret = r;
			(*this)(ret.o, &ret.o);
			(*this)(ret.d, &ret.d);
			return ret;
		}
		void operator()(const JRay& r, JRay* pOut) const {
			(*this)(pOut->o, &pOut->o);
			(*this)(pOut->d, &pOut->d);
		}
		JRayDifferential operator()(const JRayDifferential& r) const {
			JRay tempR = (*this)(JRay(r));
			JRayDifferential result(tempR.o, tempR.d, tempR.tmax, tempR.time, tempR.depth);
			result.hasDifferentials = r.hasDifferentials;
			(*this)(r.rxO, &result.rxO);
			(*this)(r.ryO, &result.ryO);
			(*this)(r.rxD, &result.rxD);
			(*this)(r.ryD, &result.ryD);
			return result;
		}
		void operator()(const JRayDifferential& r, JRayDifferential* pOut) const {
			(*this)(JRay(r), pOut);
			pOut->hasDifferentials = r.hasDifferentials;
			(*this)(r.rxO, &pOut->rxO);
			(*this)(r.ryO, &pOut->ryO);
			(*this)(r.rxD, &pOut->rxD);
			(*this)(r.ryD, &pOut->ryD);
		}
		JBound3 operator()(const JBound3& b) const {
			const Transform& M = *this;
			JBound3 result(M(JPoint3(b.pMin)));
			result = Union(result, M(JPoint3(b.pMax.mData.x, b.pMin.mData.y, b.pMin.mData.z)));
			result = Union(result, M(JPoint3(b.pMin.mData.x, b.pMax.mData.y, b.pMin.mData.z)));
			result = Union(result, M(JPoint3(b.pMin.mData.x, b.pMin.mData.y, b.pMax.mData.z)));
			result = Union(result, M(JPoint3(b.pMin.mData.x, b.pMax.mData.y, b.pMax.mData.z)));
			result = Union(result, M(JPoint3(b.pMax.mData.x, b.pMax.mData.y, b.pMin.mData.z)));
			result = Union(result, M(JPoint3(b.pMax.mData.x, b.pMin.mData.y, b.pMax.mData.z)));
			result = Union(result, M(JPoint3(b.pMax.mData.x, b.pMax.mData.y, b.pMax.mData.z)));
			return result;
		}
	public:
		std::string ToString() const {
			std::string result;
	
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					result += std::to_string(mData.m[i][j]) + " ";
				}
				result += "\n";
			}
			return result;
		}
	public:
		SimMatrix GetSIMD() const {
			return DirectX::XMLoadFloat4x4(&mData);
		}
		void SetSIMD(FSimMatrix m) {
			DirectX::XMStoreFloat4x4(&mData, m);
		}
	};

}

