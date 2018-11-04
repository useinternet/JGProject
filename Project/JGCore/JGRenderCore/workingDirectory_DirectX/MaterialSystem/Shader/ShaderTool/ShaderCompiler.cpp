#include"ShaderCompiler.h"
#include"DirectX/DirectX.h"
#include"JGRenderCore.h"
#include"MaterialSystem/Data/ShaderData.h"
using namespace JGRC;
using namespace std;
HWND ShaderCompiler::hWnd;
string ShaderCompiler::FunctionName  = "main";
string ShaderCompiler::VSVersion     = "vs_5_0";
string ShaderCompiler::PSVersion     = "ps_5_0";
CompileFlag  ShaderCompiler::Flags   = D3D10_SHADER_ENABLE_STRICTNESS;
DirectX* ShaderCompiler::m_pDirectX = nullptr;


ShaderCompiler::ShaderCompiler(const std::wstring& hlslPath, ShaderObject* object, InputLayoutData* data)
{
	m_pDirectX = DirectX::GetInstance();
	Success = Compile(hlslPath, object,data);
}
ShaderCompiler::~ShaderCompiler()
{

}
bool ShaderCompiler::Compile(const std::wstring& hlslPath, ShaderObject* object, InputLayoutData* data)
{
	ID3D10Blob* Shader = nullptr;
	ID3D10Blob* Error  = nullptr;
	// 버전 체크
	string version;
	switch (object->Type)
	{
	case EShaderType::Pixel:
		version = PSVersion;
		break;
	case EShaderType::Vertex:
		version = VSVersion;
		break;
	}
	// 컴파일
	HRESULT result = S_OK;
	result = D3DX11CompileFromFile(hlslPath.c_str(), nullptr, nullptr,
		FunctionName.c_str(), version.c_str(), Flags, 0, nullptr, &Shader, &Error, nullptr);
	if (FAILED(result))
	{
		char* compileErrors;
		if (Error)
		{
			compileErrors = (char*)(Error->GetBufferPointer());
		}
		else
		{
			MessageBox(hWnd, hlslPath.c_str(), TT("Missing Shader File"), MB_OK);
		}
		return false;
	}
	// 버퍼 생성
	switch (object->Type)
	{
	case EShaderType::Vertex:
		result = m_pDirectX->GetDevice()->CreateVertexShader( Shader->GetBufferPointer(), Shader->GetBufferSize(),
			nullptr, (ID3D11VertexShader**)&(object->Buffer));
		break;
	case EShaderType::Pixel:
		result = m_pDirectX->GetDevice()->CreatePixelShader(Shader->GetBufferPointer(), Shader->GetBufferSize(),
			nullptr, (ID3D11PixelShader**)&(object->Buffer));
		break;
	default:
		break;
	}
	// 
	if (FAILED(result))
	{
		return false;
	}

	// 레이아웃 생성
	if (data)
	{
		m_pDirectX->GetDevice()->CreateInputLayout(data->Get(), (UINT)data->vDescs.size(),
			Shader->GetBufferPointer(), Shader->GetBufferSize(), &(object->InputLayout));
	}
	return true;
}


void ShaderCompiler::ShaderInitialSetting(HWND hwnd, const string& functionName, const string& shaderVersion,
	const CompileFlag flag)
{
	hWnd = hwnd;
	FunctionName  = functionName;
	VSVersion     = "vs_" + shaderVersion;
	PSVersion     = "ps_" + shaderVersion;
	Flags = flag;
}