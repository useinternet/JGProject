
struct GUIObject
{
    int2 size;
};

StructuredBuffer<GUIObject> g_GUIObjects : register(t0, space0);

struct VSInput
{
    float3 posL     : POSITION;
    float4 color    : COLOR;
    float2 texCoord : TEXCOORD;
    
};

struct VSOutput
{
    float4 posH  : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    uint   instanceID : INSTANCE;
};


VSOutput main(VSInput input, uint instanceID : SV_InstanceID)
{
	VSOutput output;
	


    return output;
}
