



struct PointLight
{
    float3 Position;
    float  Range;
    float3 Color;
    float  AttRange;
    float  Intensity;
    float  Att0;
    float  Att1;
    float  Att2;
};



struct Payload
{
    float Shadow;
    uint RandSeed;
};

struct ShadowPayload
{
    float Factor;
};



RaytracingAccelerationStructure _SceneAS : register(t0, space0);
StructuredBuffer<PointLight> 	_PointLightList : register(t0, space1);


RWTexture2D<float4> _ShadowOutput : register(u0, space0);



cbuffer CB : register(b0)
{
    float4x4 _ProjMatrix;
    float4x4 _ViewMatrix;
    float4x4 _InverseProjMatrix;
    float4x4 _InverseViewMatrix;
	float3 _EyePosition;
    float  _NearZ;
    float  _FarZ;
    int    _PointLightCount;
    uint   _FrameCount;
}




uint initRand(uint val0, uint val1, uint backoff = 16)
{
	uint v0 = val0, v1 = val1, s0 = 0;

	[unroll]
	for (uint n = 0; n < backoff; n++)
	{
		s0 += 0x9e3779b9;
		v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + s0) ^ ((v1 >> 5) + 0xc8013ea4);
		v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + s0) ^ ((v0 >> 5) + 0x7e95761e);
	}
	return v0;
}
float3 getPerpendicularVector(float3 u)
{
	float3 a = abs(u);
	uint xm = ((a.x - a.y) < 0 && (a.x - a.z) < 0) ? 1 : 0;
	uint ym = (a.y - a.z) < 0 ? (1 ^ xm) : 0;
	uint zm = 1 ^ (xm | ym);
	return cross(u, float3(xm, ym, zm));
}

float nextRand(inout uint s)
{
	s = (1664525u * s + 1013904223u);
	return float(s & 0x00FFFFFF) / float(0x01000000);
}


float3 getConeSample(inout uint randSeed, float3 hitNorm, float cosThetaMax)
{

	float2 randVal = float2(nextRand(randSeed), nextRand(randSeed));

	float3 bitangent = getPerpendicularVector(hitNorm);
	float3 tangent = cross(bitangent, hitNorm);

	float cosTheta = (1.0 - randVal.x) + randVal.x * cosThetaMax;
	float r = sqrt(1.0 - cosTheta * cosTheta);
	float phi = randVal.y * 2.0 * 3.14159265f;

	return tangent * (r * cos(phi)) + bitangent * (r * sin(phi)) + hitNorm.xyz * cosTheta;
}

float3 HitWorldPosition()
{
    return WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
}




float ShootShadowRay(uint randSeed)
{
	int totalLightCount = _PointLightCount;

	float3 worldPos 	  = HitWorldPosition();
	int    lightRandIndex = min(int(nextRand(randSeed) * totalLightCount),  totalLightCount - 1);


	float shadowFactor = 0.0f;

	for(int i = 0; i< totalLightCount; ++i)
	{
		PointLight pl = _PointLightList[i];
		float  distance  = length(pl.Position - worldPos);
   		float3 direction = normalize(pl.Position - worldPos);

		ShadowPayload payload;
		payload.Factor = 0.0f;



	
		if(distance <= pl.Range)
    	{

			float3 perpL = cross(direction, float3(0.f, 1.0f, 0.f));
			// Handle case where L = up -> perpL should then be (1,0,0)
			if (all(perpL == 0.0f)) {
  				perpL.x = 1.0;
			}
			// Use perpL to get a vector from worldPosition to the edge of the light sphere
			float3 toLightEdge = normalize((pl.Position + perpL * pl.Range) - worldPos);
			// Angle between L and toLightEdge. Used as the cone angle when sampling shadow rays
			float coneAngle = acos(dot(direction, toLightEdge)) * 2.0f;

        	RayDesc ray;
        	ray.Origin    = worldPos;
        	ray.Direction = normalize(getConeSample(randSeed,direction , 0.999));;
        	ray.TMin = 0.01;
        	ray.TMax = distance;
    		TraceRay(
        		_SceneAS,
        		RAY_FLAG_ACCEPT_FIRST_HIT_AND_END_SEARCH | RAY_FLAG_SKIP_CLOSEST_HIT_SHADER,
        		0xFF,
        		1, // ShadowHit
        		0,
        		1, // ShadowMiss
        		ray,
        		payload);
    	}
		else
		{
			payload.Factor = 1.0f;
		}

		shadowFactor += payload.Factor;
	}

	shadowFactor /= totalLightCount;

	return shadowFactor;
}


