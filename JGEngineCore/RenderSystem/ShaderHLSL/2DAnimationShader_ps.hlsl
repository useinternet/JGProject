
Texture2D Texture;
SamplerState WrapSamplerState;




cbuffer SpriteBuffer
{
	float4 SpriteColor_xyz_IsSprite_w;
	float4 ColorRize;
};



struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};



float4 main(PixelInputType input) : SV_TARGET
{
	float3 SpriteColor = SpriteColor_xyz_IsSprite_w.xyz;
	float  IsSprite = SpriteColor_xyz_IsSprite_w.w;
	// 텍스쳐 컬러 가져오고..
	float4 color = Texture.Sample(WrapSamplerState, input.tex);

	// 스프라이트 컬러에 해당되는 색은 알파값 0으로 한다.
	if ( IsSprite == 1.0f)
	{
		if (color.x == SpriteColor.x && color.y == SpriteColor.y &&
			color.z == SpriteColor.z)
		{
			color.a = 0.0f;
		}
	
	}
	color = color * ColorRize;
	return color;
}