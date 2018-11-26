

Texture2D T_Pos_Depth      : register(t0);
Texture2D T_LightDepth     : register(t1);


struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};
float4 main(PixelInputType input) : SV_TARGET
{
	float Bios = 0.03f;
    float3 Color = float3(1.0f, 1.0f, 1.0f);


	return 0;
}