
#include "Common.hlsli"


RaytracingAccelerationStructure _SceneAS : register(t0, space0);
RWTexture2D<float4> _Output : register(u0, space0);


Texture2D _WorldPos    : register(t0);
Texture2D _Albedo   : register(t1);
Texture2D _Normal   : register(t2);
Texture2D _Specular   : register(t3);
Texture2D _Material : register(t4);


Texture2D _Shadow : register(t3);



cbuffer CB : register(b0)
{
    float3 _EyePosition;
    uint _FrameCount;
}


// Normal을알아야한다.
struct Payload
{
    float4 IndirectColor;
    uint rayDepth;

    
};



[shader("raygeneration")]
void RayGeneration()
{
    uint2  launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);



    uint randSeed = initRand(launchIndex.x + launchIndex.y * launchDim.x, _FrameCount, 16);


    float4 WorldPos  = _WorldPos[launchIndex];
	float3 WorldNormal = _Normal[launchIndex].xyz;
	float3 Albedo   = _Albedo[launchIndex].xyz;
	float3 Specular  = _Specular[launchIndex].xyz;
	float  Roughness  = _Material[launchIndex].x;
	float  Shadow    = _Shadow[launchIndex];

    

    float3 V = normalize(_EyePosition - WorldPos.xyz);
    float3 N = normalize(WorldNormal);
    if (dot(N, V) <= 0.0f) 
    {
        N = -N;
    }
    float NdotV = dot(N, V);

    bool isGeometryValid = (WorldPos.w >= 0.0f);
	float3 shadeColor;
	float3 bounceColor;
    if(isGeometryValid)
    {

    }


}
[shader("closesthit")]
void ClosestHit(inout Payload payload, BuiltInTriangleIntersectionAttributes attribute)
{

}
[shader("miss")]
void Miss(inout Payload payload : SV_RayPayload)
{

}


