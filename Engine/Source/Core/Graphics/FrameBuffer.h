#pragma once
#include "JGCore.h"
#include "GraphicsDefine.h"



namespace JG
{
	class  ITexture;
	struct FrameBufferInfo
	{
		handle Handle = 0;
		Color   ClearColor;
		ETextureFormat Format = ETextureFormat::None;
		u32 Width  = 0;
		u32 Height = 0;
		
		FrameBufferInfo() = default;
		FrameBufferInfo(
			handle handle,
			Color clearColor,
			ETextureFormat format,
			u32 width,
			u32 height
		) : Handle(handle),
			Format(format),
			Width(width),
			Height(height),
			ClearColor(clearColor) {}

	};
	class  IFrameBuffer
	{
	protected:
		virtual bool Init(const FrameBufferInfo& settings) = 0;
		virtual void Update() = 0;
		virtual bool Present() = 0;
	public:
		virtual void SubmitTexture(SharedPtr<ITexture> texture) = 0;
		virtual void Resize(u32 width, u32 height)		    = 0;
		virtual const FrameBufferInfo& GetInfo() const = 0;
	public:
		virtual ~IFrameBuffer() = default;
	public:
		static SharedPtr<IFrameBuffer> Create(const FrameBufferInfo& info);
	};
}


