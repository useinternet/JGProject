#pragma once
#include"Common/JGRCCommon.h"
#include"DirectXCommon.h"
/*
기본 머터리얼 viewDirection


*/


// 카메라 정보
/*
머터리얼
normal
diff(albedo) <-
specExp, <- 정반사광 exp
specIntensity <- 빛 강도
*/
/* 필요 변수
1. LightDir
2. LightColor
3. 

*/
class CORE_EXPORT Light
{
public:
	jgVec3 AmbientDown;
	jgVec3 AmbientUp;
};