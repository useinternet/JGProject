#include"Quaternion.h"
#include"Transform.h"
using namespace JGLibrary;
using namespace DirectX;
Quaternion::Quaternion(const Transform& form) {
	SimVector v = XMQuaternionRotationMatrix(form.GetSIMD());
	SetSIMD(v);
}
Transform Quaternion::ToTransform() const {
	SimMatrix m = XMMatrixRotationQuaternion(GetSIMD());
	return Transform(m);
}

