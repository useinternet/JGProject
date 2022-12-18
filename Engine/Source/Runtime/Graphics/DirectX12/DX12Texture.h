#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"
#include "Classes/Texture.h"
#include "JGGraphicsObject.h"
#include "DirectX12/Classes/DirectX12Helper.h"

class PDX12Texture 
	: public IMemoryObject 
	, public ITexture
	, public IJGGraphicsObject
{
	friend class PDirectX12API;

	HDX12ComPtr<HDX12Resource> _dx12Resource;
	HTextureInfo _textureInfo;

public:
	virtual ~PDX12Texture() = default;

public:
	virtual uint64 GetTextureID() const override;
	virtual const HTextureInfo& GetTextureInfo() const override;
	virtual const char* GetPixels() const override;

	virtual void Reset() override;
	virtual bool IsValid() const override;

	HDX12Resource* Get() const;
protected:
	virtual void Initialize(const HTextureInfo& inTextureInfo) override;
	virtual void InitializeByMemory(const char* pixels, const HTextureInfo& inTextureInfo) override;
};