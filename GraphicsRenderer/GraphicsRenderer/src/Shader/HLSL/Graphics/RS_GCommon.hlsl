#include"GraphicsStructDefine.hlsl"

#ifndef __COMMON_ROOTSIGNATURE_DEFINE_HLSL__
#define __COMMON_ROOTSIGNATURE_DEFINE_HLSL__




#define NUM_TEXTURESLOT   8




// 오브젝트 정의
StructuredBuffer<ObjectCB> gObjectCBs : register(t0, space0);

// 상수 버퍼 
cbuffer PassCBuffer : register(b0, space0)
{
    PassCB gPassCB;
}


// 텍스쳐 정의
Texture2D   gTexture[NUM_TEXTURESLOT] : register(t0, space1);
TextureCube gCubeTexture[NUM_TEXTURESLOT] : register(t0, space2);


// 샘플러 정의
SamplerState gAnisotropicWrap   :  register(s0, space0);
SamplerState gAnisotropicClamp  : register(s1, space0);
SamplerState gLinearWrap        : register(s2, space0);
SamplerState gLinearClamp       : register(s3, space0);
SamplerState gPointWrap         : register(s4, space0);
SamplerState gPointClamp        : register(s5, space0);
#endif