#pragma once
#include "Math/JVector.h"
#include "Math/JMatrix.h"
#include "Data/Color.h"
class PaperVertex
{
public:
	JVector3 position;
	JVector2 texcoord;
	PaperVertex() {}
	PaperVertex(float p1, float p2, float p3,
		float tex1, float tex2) :
		position(p1, p2, p3), texcoord(tex1, tex2) {}
	PaperVertex(const JVector3& pos, const JVector2& tex) :
		position(pos), texcoord(tex) {}
};
class StaticVertex
{
public:
	JVector3 position;
	JVector3 normal;
	JVector3 tangent;
	JVector2 texcoord;
	StaticVertex() {}
	StaticVertex(
		float p1, float p2, float p3,
		float n1, float n2, float n3,
		float t1, float t2, float t3,
		float tex1, float tex2) :
		position(p1, p2, p3), normal(n1, n2, n3), tangent(t1, t2, t3), texcoord(tex1, tex2) {}
	StaticVertex(
		const JVector3& pos,
		const JVector3& normal,
		const JVector3& tangent,
		const JVector2& tex) :
		position(pos), normal(normal), tangent(tangent), texcoord(tex) {}
};
class SkinnedVertex
{
public:
	JVector3 position;
	JVector3 normal;
	JVector3 tangent;
	JVector2 texcoord;
	JVector4 boneWeights;
	JVector4Int boneID;

	SkinnedVertex() {}
	SkinnedVertex(
		float p1, float p2, float p3,
		float n1, float n2, float n3,
		float t1, float t2, float t3,
		float tex1, float tex2,
		float w1, float w2, float w3, float w4,
		int b1, int b2, int b3, int b4) :
		position(p1, p2, p3), normal(n1, n2, n3), tangent(t1, t2, t3), texcoord(tex1, tex2),
		boneWeights({ w1,w2,w3,w4 }), boneID({ b1, b2, b3, b4 }) {}

	SkinnedVertex(
		const JVector3& pos,
		const JVector3& normal,
		const JVector3& tangent,
		const JVector2& tex,
		const JVector4& bone_weights,
		const JVector4Int& bone_ids) :
		position(pos), normal(normal), tangent(tangent), texcoord(tex),
		boneWeights(bone_weights), boneID(bone_ids) {}


};
class SDObject
{
public:
	JMatrix world = JMatrix::Identity();;
};
enum
{
	CBAnimData_Transform_Count = 128
};
class CBAnimData
{
public:
	void Reset()
	{
		for (int i = 0; i < CBAnimData_Transform_Count; ++i)
		{
			transform[i] = JMatrix::Identity();
		}
	}
public:
	JMatrix transform[CBAnimData_Transform_Count];
};

class CBCamera 
{
public:
	JMatrix view        = JMatrix::Identity();
	JMatrix proj        = JMatrix::Identity();
	JMatrix viewProj    = JMatrix::Identity();
	JMatrix invView     = JMatrix::Identity();
	JMatrix invProj     = JMatrix::Identity();
	JMatrix invViewProj = JMatrix::Identity();


	float lensWidth  = 0.0f;
	float lensHeight = 0.0f;
	float farZ       = 0.0f;
	float nearZ      = 0.0f;

	JVector3 eyePos;
};
class SDPassData
{
public:
	uint32_t PointLightCount = 0;
	uint32_t SpotLightCount  = 0;
	uint32_t DirectionalLightCount = 0;
	uint32_t ShadowFilter    = 0;
};
class SDAmbientLight
{

public:
	JVector4 ambient;
};

class SDPointLight
{
public:
	JMatrix proj;
	JMatrix viewProj[6];

	JVector4 diffuse;

	JVector3 position;
	float att0 = 1.0f;

	float att1 = 0.0f;
	float att2 = 0.0f;
	float inRange = 0.0f;
	float outRange = 0.0f;
	
	float depthBias = 0.00001f;
};
class SDSpotLight
{
public:
	JMatrix  viewProj;
	JVector4 diffuse;

	JVector3 position;
	float att0 = 1.0f;
	float att1 = 0.0f;
	float att2 = 0.0f;
	float inCone = 0.0f;
	float outCone = 0.0f;

	JVector3 dir;
	float  inRange;

	float  outRange;
	float depthBias = 0.00001f;
};
class SDDirectionalLight
{
public:
	JMatrix viewProj;
	JColor   diffuse;
	JVector3 dir;
	float padding;
	JVector4 depthBias;
	JVector4 cascadeOffsetX;
	JVector4 cascadeOffsetY;
	JVector4 cascadeScale;
};
