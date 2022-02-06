




struct Material
{
    float3 WorldPosition;
    float3 Albedo;
    float  Roughness;
    float  Metallic;
    float3 Emissive;
};

struct DirectionalLight
{
    float3 Direction;
    float3 LightColor;
};

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





struct LightGrid
{
    int PL_Count;
    int PL_Offset;
};

struct Cluster
{
    float4 Min;
    float4 Max;
};

struct Vertex
{
	float3 Position;
	float2 Texcoord;
	float3 Normal;
	float3 Tangent;
	float3 Bitangent;
};

struct SURFACE_OUTPUT
{
	float4 albedo;
	float3 normal;
	float3 specular;
	float roughness;
	float metallic;
	float3 emissive;
    int shadingmodel;
};
struct SURFACE_INPUT
{
	float3 position;
	float3 local_position;
	float3 normal;
	float3 tangent;
	float3 bitangent;
	float2 tex;
};


float3 HitWorldPosition()
{
    return WorldRayOrigin() + RayTCurrent() * WorldRayDirection();
}

// Retrieve hit object space position.
float3 HitObjectPosition()
{
    return ObjectRayOrigin() + RayTCurrent() * ObjectRayDirection();
}
float3 HitAttribute(float3 vertexAttribute[3], BuiltInTriangleIntersectionAttributes attr)
{
    return vertexAttribute[0] +
        attr.barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]) +
        attr.barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
}
float2 HitAttribute(float2 vertexAttribute[3], BuiltInTriangleIntersectionAttributes attr)
{
    return vertexAttribute[0] +
        attr.barycentrics.x * (vertexAttribute[1] - vertexAttribute[0]) +
        attr.barycentrics.y * (vertexAttribute[2] - vertexAttribute[0]);
}
Vertex HitAttribute(Vertex vertices[3], BuiltInTriangleIntersectionAttributes attr)
{
    float2 vertexTexcoords[3] = { vertices[0].Texcoord, vertices[1].Texcoord, vertices[2].Texcoord };
    float2 texCoord = HitAttribute(vertexTexcoords, attr);
    float orientation = HitKind() == HIT_KIND_TRIANGLE_FRONT_FACE ? 1 : -1;

    float3 vertexNormals[3] = { vertices[0].Normal, vertices[1].Normal, vertices[2].Normal };
    float3 normal = normalize(HitAttribute(vertexNormals, attr));

    normal *= orientation;
    normal = normalize(mul((float3x3)ObjectToWorld3x4(), normal));


    float3 vertexTangents[3] = { vertices[0].Tangent, vertices[1].Tangent, vertices[2].Tangent };
    float3 tangent = normalize(HitAttribute(vertexTangents, attr));
    tangent *= orientation;
    tangent = normalize(mul((float3x3)ObjectToWorld3x4(), tangent));

    float3 vertexBitangents[3] = { vertices[0].Bitangent, vertices[1].Bitangent, vertices[2].Bitangent };
    float3 bitangent = normalize(HitAttribute(vertexBitangents, attr));
    bitangent *= orientation;
    bitangent = normalize(mul((float3x3)ObjectToWorld3x4(), bitangent));



    Vertex result;
    result.Position  = HitWorldPosition();
    result.Texcoord  = texCoord;
    result.Tangent   = tangent;
    result.Bitangent = bitangent;
    result.Normal    = normal;
    return result;
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


struct HaltonState
{
    uint dimension;
    uint sequenceIndex;
};
float haltonSample(uint dimension, uint sampleIndex);
float haltonNext(inout HaltonState state);
uint haltonIndex(uint x, uint y, uint i);
uint halton2Inverse(uint index, uint digits);
uint halton3Inverse(uint index, uint digits);

void haltonInit(inout HaltonState hState,
    int x, int y,
    int path, int numPaths,
    int frameId,
    int loop)
{
    hState.dimension = 2;
    hState.sequenceIndex = haltonIndex(x, y,
        (frameId * numPaths + path) % (loop * numPaths));
}

float haltonSample(uint dimension, uint sampleIndex)
{
    int base = 0;

    // Use a prime number.
    switch (dimension)
    {
    case 0:  base = 2;   break;
    case 1:  base = 3;   break;
    case 2:  base = 5;   break;
    case 3:  base = 7;   break;
    case 4:  base = 11;   break;
    case 5:  base = 13;   break;
    case 6:  base = 15;   break;
    case 7:  base = 17;   break;
    case 8:  base = 19;   break;
    case 9:  base = 23;   break;
    case 10:  base = 29;   break;
    case 11:  base = 31;   break;
    case 12:  base = 37;   break;
    case 13:  base = 41;   break;
    case 14:  base = 43;   break;
    case 15:  base = 47;   break;
    case 16:  base = 53;   break;
    case 17:  base = 59;   break;
    case 18:  base = 61;   break;
    case 19:  base = 67;   break;
    case 20:  base = 71;   break;
    case 21:  base = 73;   break;
    case 22:  base = 79;   break;
    case 23:  base = 83;   break;
    case 24:  base = 89;   break;
    case 25:  base = 97;   break;
    case 26:  base = 101;   break;
    case 27:  base = 103;   break;
    case 28:  base = 109;   break;
    case 29:  base = 113;   break;
    case 30:  base = 127;   break;
    case 31: base = 131; break;
    default: base = 2;   break;
    }

    // Compute the radical inverse.
    float a = 0;
    float invBase = 1.0f / float(base);

    for (float mult = invBase;
        sampleIndex != 0; sampleIndex /= base, mult *= invBase)
    {
        a += float(sampleIndex % base) * mult;
    }

    return a;
}

float haltonNext(inout HaltonState state)
{
    return haltonSample(state.dimension++, state.sequenceIndex);
}

// Modified from [pbrt]
uint haltonIndex(uint x, uint y, uint i)
{
    return ((halton2Inverse(x % 256, 8) * 76545 +
        halton3Inverse(y % 256, 6) * 110080) % 3) + i * 186624;
}

// Modified from [pbrt]
uint halton2Inverse(uint index, uint digits)
{
    index = (index << 16) | (index >> 16);
    index = ((index & 0x00ff00ff) << 8) | ((index & 0xff00ff00) >> 8);
    index = ((index & 0x0f0f0f0f) << 4) | ((index & 0xf0f0f0f0) >> 4);
    index = ((index & 0x33333333) << 2) | ((index & 0xcccccccc) >> 2);
    index = ((index & 0x55555555) << 1) | ((index & 0xaaaaaaaa) >> 1);
    return index >> (32 - digits);
}

// Modified from [pbrt]
uint halton3Inverse(uint index, uint digits)
{
    uint result = 0;
    for (uint d = 0; d < digits; ++d)
    {
        result = result * 3 + index % 3;
        index /= 3;
    }
    return result;
}










