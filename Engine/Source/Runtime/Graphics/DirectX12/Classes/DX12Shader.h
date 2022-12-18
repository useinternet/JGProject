#pragma once
#include "Core.h"
#include "JGGraphicsDefine.h"
#include "DirectX12Helper.h"

class IComputeBuffer;
class GraphicsPipelineState;
class RootSignature;
class ITexture;
class ShaderScriptCodeAnalyzer;

struct HCompileConfig
{
	PString Entry;
	PString Target;
	HCompileConfig(const PString& entry, const PString& target) : Entry(entry), Target(target) {}
};

class PDX12Shader : public IMemoryObject
{
protected:
	EShaderFlags _flags;
	bool _bSuccess = false;

public:
	virtual ~PDX12Shader() = default;

	EShaderFlags GetFlags() const
	{
		return _flags;
	}

	bool IsSuccessed() const
	{
		return _bSuccess;
	}

protected:
	bool compile(HDX12ComPtr<HDX12Blob>& blob, const PString& sourceCode, const HCompileConfig& config, PString* error);
};

class PDX12GraphicsShader : public PDX12Shader
{
	HDX12ComPtr<HDX12Blob> _VSData;
	HDX12ComPtr<HDX12Blob> _DSData;
	HDX12ComPtr<HDX12Blob> _HSData;
	HDX12ComPtr<HDX12Blob> _GSData;
	HDX12ComPtr<HDX12Blob> _PSData;
public:
	virtual ~PDX12GraphicsShader() = default;

	HDX12Blob* GetVSData() const {
		if (IsSuccessed() == false)
		{
			return nullptr;
		}
		return _VSData.Get();
	}
	HDX12Blob* GetDSData() const {
		if (IsSuccessed() == false)
		{
			return nullptr;
		}
		return _DSData.Get();
	}
	HDX12Blob* GetHSData() const {
		if (IsSuccessed() == false)
		{
			return nullptr;
		}
		return _HSData.Get();
	}
	HDX12Blob* GetGSData() const {
		if (IsSuccessed() == false)
		{
			return nullptr;
		}
		return _GSData.Get();
	}
	HDX12Blob* GetPSData() const {
		if (IsSuccessed() == false)
		{
			return nullptr;
		}
		return _PSData.Get();
	}

	bool Compile(const PString& code, EShaderFlags flags, PString* error);
};

class PDX12ComputeShader : public PDX12Shader
{
	HDX12ComPtr<HDX12Blob> _CSData;

public:
	virtual ~PDX12ComputeShader() = default;

	bool Compile(const PString& code, PString* error);

	HDX12Blob* GetCSData() const {
		if (IsSuccessed() == false)
		{
			return nullptr;
		}

		return _CSData.Get();
	}
};
