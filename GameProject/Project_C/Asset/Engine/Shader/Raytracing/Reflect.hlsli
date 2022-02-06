
//#include "Common.hlsli"



[shader("raygeneration")]
void RayGeneration()
{
    uint2  launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (((launchIndex.xy + 0.5f) / dims.xy) * 2.f - 1.f);

    RayDesc ray;
    ray.Origin = _EyePosition;
    float4 target = mul(float4(d.x, -d.y, 1, 1), _InvProjMatrix);
    ray.Direction = mul(float4(target.xyz, 0), _InvViewMatrix);
    ray.TMin = 0;
    ray.TMax = _FarZ;

    Payload payload;
    payload.Color = float4(0.0f,0.0f,0.0f,0.0f);


    TraceRay(
        _SceneAS,
        RAY_FLAG_NONE,
        0xFF,
        0,
        1,
        0,
        ray,
        payload);


    _Output[launchIndex] = payload.Color;
}


// Local //
cbuffer _DefaultCB__ : register(b0, space1)
{ }
//ConstantBuffer<MaterialCB> Test : register(b0, space1);
StructuredBuffer<Vertex> Local_VertexBuffer   : register(t0, space1);
StructuredBuffer<uint>   Local_IndexBuffer    : register(t1, space1);
Texture2D Null_Texture : register(t2, space1);
// Default Hit Shader
[shader("closesthit")]
void ClosestHit(inout Payload payload, BuiltInTriangleIntersectionAttributes attribute)
{
    uint startIndex = PrimitiveIndex() * 3;
    const uint3 indices = { Local_IndexBuffer[startIndex], Local_IndexBuffer[startIndex + 1], Local_IndexBuffer[startIndex + 2] };
    Vertex vertices[3] = {
        Local_VertexBuffer[indices[0]],
        Local_VertexBuffer[indices[1]],
        Local_VertexBuffer[indices[2]] };

    Vertex v = HitAttribute(vertices, attribute);

    float4 textureColor = Null_Texture.SampleLevel(_PointWrap, v.Texcoord, 0);
    payload.Color = textureColor;
}

// Default Miss Shader
[shader("miss")]
void Miss(inout Payload payload : SV_RayPayload)
{
    payload.Color = float4(0.0f,0.0f,0.0f,0.0f);
}

