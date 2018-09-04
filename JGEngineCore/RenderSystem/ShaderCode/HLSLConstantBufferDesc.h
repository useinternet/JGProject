#pragma once
#include"../../EngineStatics/Engine.h"


typedef struct SMatrixBuffer_VS
{
	D3DXMATRIX WorldViewProjectionMatrix;
}SMatrixBuffer;

typedef struct S2DSpriteBuffer_PS
{
	D3DXVECTOR3 SprietColor = { 0.0f,0.0f,0.0f };
	float IsSprite = 0.0f;
}S2DSpriteBuffer;

typedef struct STextBuffer_PS
{

}STextBuffer;