#pragma once
#include "Math/JVector.h"



struct JVertex
{
	JVector3 Position;
	JVector3 Normal;
	JVector3 Tangent;
	JVector2 Texcoord;
public:
	JVertex() : Position(0, 0, 0), Normal(0, 0, 0), Tangent(0, 0, 0), Texcoord(0, 0) {}
	JVertex(const JVector3& pos, const JVector3& normal, const JVector3& tangent, const JVector2& tex) :
		Position(pos), Normal(normal), Tangent(tangent), Texcoord(tex) {}
	JVertex(float p1, float p2, float p3, float n1, float n2, float n3, float tan1, float tan2, float tan3, float t1, float t2) :
		Position({ p1,p2,p3 }), Normal({ n1,n2,n3 }), Tangent({ tan1, tan2, tan3 }), Texcoord(t1,t2) {}
	JVertex(const JVertex& copy) = default;


	JVertex& operator=(const JVertex& rhs) = default;
	JVertex& operator=(JVertex&& rhs) = default;
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



