#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"


struct HFrameBufferInfo
{
	uint64  Handle = 0;
	HColor  ClearColor;
	ETextureFormat Format = ETextureFormat::None;
	uint32 Width  = 0;
	uint32 Height = 0;

	HFrameBufferInfo() = default;
	HFrameBufferInfo(
		uint64 handle,
		HColor clearColor,
		ETextureFormat format,
		uint32 width,
		uint32 height
	) : Handle(handle),
		Format(format),
		Width(width),
		Height(height),
		ClearColor(clearColor) {}

};

class  IFrameBuffer
{
protected:
	virtual bool Init(const HFrameBufferInfo& settings) = 0;
	virtual void Update()  = 0;
	virtual bool Present() = 0;

public:
	//virtual void SubmitTexture(SharedPtr<ITexture> texture) = 0;
	virtual void Resize(uint32 width, uint32 height) = 0;
	virtual const HFrameBufferInfo& GetInfo() const  = 0;

public:
	virtual ~IFrameBuffer() = default;
};