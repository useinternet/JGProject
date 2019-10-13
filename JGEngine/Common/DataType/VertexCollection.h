#pragma once
#include "Math/JVector.h"



struct JVertex
{
	JVector3 Position;
	JVector3 Normal;
	JVector3 Tangent;
	JVector2 Texcoord;
};

struct JSkinnedVertex
{
	JVector3 Position;
	JVector3 Normal;
	JVector3 Tangent;
	JVector2 Texcoord;
	float boneWeights[4] = { 0.0f,0.0f,0.0f,0.0f };
	uint32_t bondId[4]   = { 0,0,0,0 };
};



