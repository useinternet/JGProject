#pragma once
#include"../../EngineStatics/Engine.h"

class JGDeviceD;


class ENGINE_EXPORT JGShaderCompiler
{
private:
	Microsoft::WRL::ComPtr<ID3D10Blob> m_VertexShaderBuffer;
	Microsoft::WRL::ComPtr<ID3D10Blob> m_PixelShaderBuffer;
	ID3D11VertexShader** pVertexShaderAddress;
	ID3D11PixelShader**  pPixelShaderAddress;
	ID3D11InputLayout**  pInputLayoutAddress;
	D3D11_INPUT_ELEMENT_DESC* pElementDesc;
	UINT ElementSize = 0;
	std::string EntryPoint;
	std::string VSVersion;
	std::string PSVersion;
public:
	JGShaderCompiler() = delete;
	JGShaderCompiler(const JGShaderCompiler& copy) = delete;
	JGShaderCompiler(ID3D11VertexShader** VertexShader, ID3D11PixelShader** PixelShader,
		D3D11_INPUT_ELEMENT_DESC* elementDesc, UINT elementSize, ID3D11InputLayout** InputLayout,
		const std::string& entryPoint,const std::string& Version);
	~JGShaderCompiler();

	bool Compile(JGDeviceD* Device, const wchar_t* VertexHlslPath, const wchar_t* PixelHlslPath);
};