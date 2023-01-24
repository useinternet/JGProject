#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"

class IGUIBuild;
class IJGGui
{
public:
	virtual uint64 GPUAllocate(TextureID textureID) = 0;
};