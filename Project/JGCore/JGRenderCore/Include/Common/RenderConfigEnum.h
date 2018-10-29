#pragma once
#include"Common/dllExportDefine.h"

namespace JGRC
{
	/*
	EnumClass : EDepthStateType
	@e ZBufferOn  : Z축을 On한다.
	@e ZBufferOff : Z축을 Off한다.*/
	enum class CORE_EXPORT EDepthStateType
	{
		ZBufferOn = 0,
		ZBufferOff = 1
	};
	/*
	EnumClass : EBlendStateType
	@e BlendOn  : 알파 값을 적용한다.
	@e BlendOff : 알파 값을 적용하지 않는다.*/
	enum class CORE_EXPORT EBlendStateType
	{
		BlendOn = 0,
		BlendOff = 1
	};
	/*
	EnumClass : EFillModeType
	@e Solid     : 정점 사이에 색을 모두 채운다.
	@e WireFrame : 정점 사이에 색을 모두 채우지 않는다.*/
	enum class CORE_EXPORT EFillModeType
	{
		Solid = 0,
		WireFrame = 1
	};

}