#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"

class HJGGraphicsArguments
{
public:
	uint64 Handle;
	int32  BufferCount;
	int32  Width;
	int32  Height;
	HLinearColor ClearColor;

	HJGGraphicsArguments()
		: BufferCount(2)
		, Width(0)
		, Height(0) {}

};

class ITexture;
class IJGGui;
class PJGGraphicsAPI : public IMemoryObject
{
	friend class HJGGraphicsModule;
public:
	virtual ~PJGGraphicsAPI() = default;

protected:
	virtual void Initialize(const HJGGraphicsArguments& args) = 0;
	virtual void BeginFrame() = 0;
	virtual void EndFrame() = 0;
public:
	virtual IJGGui* GetGui() const = 0;
	virtual PSharedPtr<ITexture> CreateTexture(const HTextureInfo& textureInfo) = 0;
	virtual PSharedPtr<ITexture> CreateTexture(const char* pixels, const HTextureInfo& textureInfo) = 0;


};