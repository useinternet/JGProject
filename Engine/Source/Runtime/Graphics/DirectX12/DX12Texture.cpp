#include "PCH/PCH.h"
#include "DX12Texture.h"
#include "JGGraphicsHelper.h"
#include "DirectX12/DirectX12API.h"
#include "Classes/CommandList.h"

uint64 PDX12Texture::GetTextureID() const
{
	return uint64();
}

const HTextureInfo& PDX12Texture::GetTextureInfo() const
{
	return _textureInfo;
}

const char* PDX12Texture::GetPixels() const
{
	return nullptr;
}

void PDX12Texture::Reset()
{
}

bool PDX12Texture::IsValid() const
{
	return _dx12Resource != nullptr;
}

HDX12Resource* PDX12Texture::Get() const
{
	return _dx12Resource.Get();
}

void PDX12Texture::Initialize(const HTextureInfo& inTextureInfo)
{
	Reset();

	_textureInfo = inTextureInfo;

	ETextureFlags flags = inTextureInfo.Flags;

	D3D12_RESOURCE_FLAGS d3dRscFlags = D3D12_RESOURCE_FLAG_NONE;
	if (flags & ETextureFlags::Allow_RenderTarget)
	{
		d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}
	else if (flags & ETextureFlags::Allow_DepthStencil)
	{
		d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	}
	if (flags & ETextureFlags::Allow_UnorderedAccessView)
	{
		d3dRscFlags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	D3D12_RESOURCE_DESC rscDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		HDirectX12Helper::ConvertDXGIFormat(_textureInfo.Format), _textureInfo.Width, _textureInfo.Height,
		_textureInfo.ArraySize, _textureInfo.MipLevel, 1, 0,
		d3dRscFlags, D3D12_TEXTURE_LAYOUT_UNKNOWN, 0);

	bool bUseClearValue = false;

	D3D12_CLEAR_VALUE clearValue;
	if (flags & ETextureFlags::Allow_RenderTarget)
	{
		bUseClearValue = true;

		clearValue.Color[0] = _textureInfo.ClearColor.R;
		clearValue.Color[1] = _textureInfo.ClearColor.G;
		clearValue.Color[2] = _textureInfo.ClearColor.B;
		clearValue.Color[3] = _textureInfo.ClearColor.A;
		clearValue.Format = HDirectX12Helper::ConvertDXGIFormat(_textureInfo.Format);
	}
	else if (flags & ETextureFlags::Allow_DepthStencil)
	{
		bUseClearValue = true;

		clearValue.DepthStencil.Depth   = _textureInfo.ClearDepth;
		clearValue.DepthStencil.Stencil = _textureInfo.ClearStencil;
		clearValue.Format = HDirectX12Helper::ConvertDXGIFormat(_textureInfo.Format);
	}

	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	_dx12Resource = HDirectXAPI::CreateCommittedResource(
		_textureInfo.Name,
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&rscDesc,
		D3D12_RESOURCE_STATE_COMMON,
		bUseClearValue ? &clearValue : nullptr);

	if (_dx12Resource)
	{
		_dx12Resource->SetName(_textureInfo.Name.GetRawWString().c_str());
	}
}

void PDX12Texture::InitializeByMemory(const char* pixels, const HTextureInfo& inTextureInfo)
{
	Initialize(inTextureInfo);

	uint8 channels = HJGGraphicsHelper::GetTextureFormatChannels(_textureInfo.Format);

	PSharedPtr<PCommandList> commandList = HDirectXAPI::RequestCommandList();
	commandList->CopyTextrueFromMemory(_dx12Resource.Get(), pixels, _textureInfo.Width, _textureInfo.Height, channels, _textureInfo.ArraySize);
}
