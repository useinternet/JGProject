#pragma once
#include"../../EngineStatics/Engine.h"



typedef struct SMatrixBuffer
{
	D3DXMATRIX worldMatrix;
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatirx;
}SMatrixBuffer;