



struct Payload
{
	float4 Color;
};

struct Built_in_attribute
{
	float BaryX;
	float BaryY;
};




[shader("closesthit")]
void Hit(inout Payload payload : SV_Payload, Built_in_attribute attr)
{
	payload.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
}


[shader("miss")]
void Miss(inout Payload payload : SV_Payload)
{
	payload.Color = 0.0f;
}
