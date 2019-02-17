#include"Transform.h"
#include"Quaternion.h"


using namespace JGLibrary;


void Transform::DeCompose(JPoint3* pos, Quaternion* quat, JPoint3* scale) {
	SimVector simpos, simquat, simscale;
	DirectX::XMMatrixDecompose(&simscale, &simquat, &simpos, GetSIMD());
	pos->SetSIMD(simpos); quat->SetSIMD(simquat); scale->SetSIMD(simscale);
}