#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"

class GRAPHICS_API ITexture
{
public:
	virtual uint64 GetTextureID() const = 0;
	virtual const HTextureInfo& GetTextureInfo() const = 0;
	virtual const char* GetPixels() const = 0;

	virtual void Reset() = 0;
	virtual bool IsValid() const = 0;
protected:
	virtual void Initialize(const HTextureInfo& inTextureInfo) = 0;
	virtual void InitializeByMemory(const char* pixels, const HTextureInfo& inTextureInfo) = 0;
};