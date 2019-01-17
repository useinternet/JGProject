

cbuffer cbBlur : register(b0)
{
    float4x4 gProj;
    float4 gBlurWeights[3];
    float2 gInvRenderTargetSize;
    float2 gPad;
};

cbuffer cbRootConstants : register(b1)
{
    bool gHorizontalBlur;
};
Texture2D gInputMap  : register(t0);
SamplerState gsamPointClamp : register(s0);
static const int gBlurRadius = 5;
static const float2 gTexCoords[6] =
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};
 
struct VertexOut
{
    float4 PosH  : SV_POSITION;
	float2 TexC  : TEXCOORD;
};

VertexOut VS(uint vid : SV_VertexID)
{
    VertexOut vout;

    vout.TexC = gTexCoords[vid];

    // Quad covering screen in NDC space.
    vout.PosH = float4(2.0f*vout.TexC.x - 1.0f, 1.0f - 2.0f*vout.TexC.y, 0.0f, 1.0f);

    return vout;
}
float4 PS(VertexOut pin) : SV_Target
{
    // unpack into float array.
    float blurWeights[12] =
    {
        gBlurWeights[0].x, gBlurWeights[0].y, gBlurWeights[0].z, gBlurWeights[0].w,
        gBlurWeights[1].x, gBlurWeights[1].y, gBlurWeights[1].z, gBlurWeights[1].w,
        gBlurWeights[2].x, gBlurWeights[2].y, gBlurWeights[2].z, gBlurWeights[2].w,
    };

	float2 texOffset;
	if(gHorizontalBlur)
	{
		texOffset = float2(gInvRenderTargetSize.x, 0.0f);
	}
	else
	{
		texOffset = float2(0.0f, gInvRenderTargetSize.y);
	}
	float4 color      = blurWeights[gBlurRadius] * gInputMap.SampleLevel(gsamPointClamp, pin.TexC, 0.0);
	float totalWeight = blurWeights[gBlurRadius];
	 
	for(float i = -gBlurRadius; i <=gBlurRadius; ++i)
	{
		if( i == 0 )
			continue;

		float2 tex = pin.TexC + i*texOffset;

        float weight = blurWeights[i + gBlurRadius];
        color += weight * gInputMap.SampleLevel(
                gsamPointClamp, tex, 0.0);
		
        totalWeight += weight;
	}
    return color / totalWeight;
}
