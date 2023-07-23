#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"
#include "Classes/FrameBuffer.h"
#include "Classes/DescriptionAllocator.h"
#include "JGGraphicsObject.h"
#include "DirectX12/Classes/DirectX12Helper.h"

class PDX12Texture;
class PDX12FrameBuffer
	: public IMemoryObject
	, public IFrameBuffer
	, public IJGGraphicsObject
{
	friend class PDirectX12API;

	HDX12ComPtr<HDX12SwapChain>		  _dx12SwapChain;
	HList<HDX12ComPtr<HDX12Resource>> _dx12BackBuffers;
	HList<HDescriptionAllocation>     _rtvs;

	PSharedPtr<PDX12Texture>   _submittedTexture;
	HFrameBufferInfo _frameBufferInfo;
public:
	JG_DECLARE_MULTICAST_DELEGATE(HOnPresent);
	JG_DECLARE_MULTICAST_DELEGATE(HOnUpdate, HDX12CommandList*, HDX12Resource*, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_RESOURCE_STATES&);
	
	HOnPresent OnPresent;
	HOnUpdate  OnUpdate;
public:
	virtual ~PDX12FrameBuffer();

protected:
	virtual bool Initialize(const HFrameBufferInfo& info) override;
	virtual void Update() override;
	virtual bool Present() override;

public:
	virtual void SubmitTexture(PSharedPtr<ITexture> inTexture) override;
	virtual void Resize(uint32 width, uint32 height) override;
	virtual const HFrameBufferInfo& GetInfo() const override;
};