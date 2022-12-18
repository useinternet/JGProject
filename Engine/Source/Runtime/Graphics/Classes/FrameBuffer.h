#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"

class ITexture;
struct HFrameBufferInfo
{
	uint64  Handle = 0;
	HLinearColor ClearColor;
	ETextureFormat Format = ETextureFormat::None;
	uint32 Width  = 0;
	uint32 Height = 0;
	uint32 FrameBufferCount = 0;

	HFrameBufferInfo() = default;
	HFrameBufferInfo(
		uint64 handle,
		uint32 bufferCount,
		HLinearColor clearColor,
		ETextureFormat format,
		uint32 width,
		uint32 height
	) : Handle(handle),
		FrameBufferCount(bufferCount),
		Format(format),
		Width(width),
		Height(height),
		ClearColor(clearColor) {}

};

class  IFrameBuffer
{
protected:
	virtual bool Initialize(const HFrameBufferInfo& info) = 0;
	virtual void Update()  = 0;
	virtual bool Present() = 0;

public:
	virtual void SubmitTexture(PSharedPtr<ITexture> inTexture) = 0;
	virtual void Resize(uint32 width, uint32 height) = 0;
	virtual const HFrameBufferInfo& GetInfo() const  = 0;
};