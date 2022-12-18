#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"

class HJGGraphicsHelper
{
public:
	static uint64 GetShaderDataTypeSize(EShaderDataType dataType);
	static PName  GetShaderDataTypeHLSLName(EShaderDataType dataType);
	static uint8  GetTextureFormatChannels(ETextureFormat format);
};