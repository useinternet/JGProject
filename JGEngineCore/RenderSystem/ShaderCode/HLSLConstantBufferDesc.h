#pragma once
#include"../../EngineStatics/Engine.h"


typedef struct SMatrixBuffer_VS
{
	D3DXMATRIX WoldViewProjectionMatrix;
}SMatrixBuffer;

typedef struct S2DSpriteBuffer_PS
{
	D3DXVECTOR3 SprietColor = { 0.0f,0.0f,0.0f };
	float IsSprite = 0.0f;
	D3DXVECTOR4 ColorRize = { 1.0f,1.0f,1.0f,1.0f };
}S2DSpriteBuffer;

typedef struct STextBuffer_PS
{
	D3DXVECTOR4 TextColor = { 1.0f,1.0f,1.0f,1.0f };
}STextBuffer;

typedef struct SAnimationIncrementBuffer_VS
{
	float WidthIncrement = 0.0f;
	float HeightIncrement = 0.0f;
	D3DXVECTOR2 Padding = { 0.0f,0.0f };
}SAnimationIncrementBuffer_VS;