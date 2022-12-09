#include "PCH/PCH.h"
#include "DX12Shader.h"
#include "DirectX12/DirectX12API.h"

bool PDX12GraphicsShader::Compile(const PString& code, EShaderFlags flags, PString* error)
{
	_flags = flags;

	if (_flags & EShaderFlags::Allow_VertexShader)
	{
		if (compile(_VSData, code, HCompileConfig(HHLSL::VSEntry, HHLSL::VSTarget), error) == false)
		{
			return false;
		}
	}
	if (_flags & EShaderFlags::Allow_DomainShader)
	{
		if (compile(_DSData, code, HCompileConfig(HHLSL::DSEntry, HHLSL::DSTarget), error) == false)
		{
			return false;
		}
	}
	if (_flags & EShaderFlags::Allow_HullShader)
	{
		if (compile(_HSData, code, HCompileConfig(HHLSL::HSEntry, HHLSL::HSTarget), error) == false)
		{
			return false;
		}
	}
	if (_flags & EShaderFlags::Allow_GeometryShader)
	{
		if (compile(_GSData, code, HCompileConfig(HHLSL::GSEntry, HHLSL::GSTarget), error) == false)
		{
			return false;
		}
	}
	if (_flags & EShaderFlags::Allow_PixelShader)
	{
		if (compile(_PSData, code, HCompileConfig(HHLSL::PSEntry, HHLSL::PSTarget), error) == false)
		{
			return false;
		}
	}

	_bSuccess = true;
	return true;
}
bool PDX12GraphicsShader::compile(HDX12ComPtr<HDX12Blob>& blob, const PString& sourceCode, const HCompileConfig& config, PString* error)
{
	ComPtr<ID3DBlob> errorData;
	HRESULT hr = D3DCompile2(
		sourceCode.GetCStr(),
		sourceCode.Length(),
		nullptr,
		nullptr,
		nullptr,
		config.Entry.GetCStr(),
		config.Target.GetCStr(),
		0, 0, 0, nullptr, 0,
		blob.GetAddressOf(),
		errorData.GetAddressOf());

	if (FAILED(hr) && error != nullptr)
	{
		*error += (char*)errorData->GetBufferPointer();
		*error += "\n";
		return false;
	}

	return true;
}


bool PDX12ComputeShader::Compile(const PString& code, const HCompileConfig& config, PString* error)
{
	_CSData = HDirectX12Helper::CompileShader("ComputeShader", code, config.Entry, config.Target);

	if (_CSData != nullptr)
	{
		_bSuccess = true;
	}

	return _bSuccess;
}
