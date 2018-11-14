#include"ShaderReader.h"
#include"DirectX/DirectX.h"
using namespace JGRC;
using namespace std;
DirectX* ShaderReader::m_Dx = nullptr;
ShaderReader::ShaderReader(HWND hWnd)
{
	m_hWnd = hWnd;
	m_Dx = DirectX::GetInstance();
}
std::vector<JGShader*> ShaderReader::ReadShader(const std::string& path)
{
	fstream fin;
	fin.open(path);
	vector<JGShader*> result;
	if (!fin.is_open())
	{
		JGLOG(log_Error, "JGRC::ShaderReader", "Failed read ShaderData   | " + path);
		return result;
	}
	string buffer;
	fin >> buffer >> buffer;
	int Count = 0;
	fin >> Count;
	for (int i = 0; i < Count; ++i)
	{
		JGShader* Shader = m_Dx->CreateObject<JGShader>();
		Read(fin,Shader);
		result.push_back(Shader);
	}
	return result;
}
void ShaderReader::Read(std::fstream& fin, JGShader* Shader)
{
	// 타입 알아내기
	string buffer;
	int num;
	fin >> buffer >> buffer;
	fin >> num;
	Shader->SetShaderType((EShaderType)num);
	// hlsl 경로
	fin >> buffer >> buffer >> buffer;
	Shader->SetPath(buffer);

	// 입력 레이아웃
	fin >> buffer;
	fin >> buffer >> buffer;
	fin >> num;
	string SemanticName; int SemanticIndex; int Format;
	int InputSlot; int Offset; int InputSlotClass;
	int StepRate;
	for (int i = 0; i < num; ++i)
	{
		fin >> SemanticName >> SemanticIndex >> Format >> InputSlot >>
			Offset >> InputSlotClass >> StepRate;
		Shader->GetLayout()->AddDesc(SemanticName.c_str(), SemanticIndex, (DXGI_FORMAT)Format,
			InputSlot, Offset, (D3D11_INPUT_CLASSIFICATION)InputSlot, InputSlotClass);
	}
	if (!Shader->Compile(m_hWnd))
	{
		JGLOG(log_Error, "JGRC::ShaderReader", "Failed ShaderCompile");
		return;
	}

	// 상수 버퍼 
	CBufferArray* arr = Shader->GetCBufferArray();
	fin >> buffer;
	fin >> buffer >> buffer >> num;
	for (int i = 0; i < num; ++i)
	{
		// 버퍼 이름
		fin >> buffer >> buffer >> buffer;
		CBuffer* Cbf = m_Dx->CreateObject<CBuffer>();
		Cbf->SetName(buffer);
		int varCount = 0;
		fin >> buffer >> buffer >> varCount;
		for (int j = 0; j < varCount; ++j)
		{
			int start; int end;
			fin >> buffer >> buffer >> buffer;
			fin >> start >> end;
			Cbf->AddVar(buffer, start, end);
		}
		Cbf->AssignMemory();
		arr->push_back(Cbf);
	}
	if (!Shader->RegisterBuffer())
	{
		JGLOG(log_Error, "JGRC::ShaderReader", "Failed Register  JGBuffer(ConstantBufferType) | " + Shader->GetPath());
	}
	// 텍스쳐
	fin >> buffer;
	fin >> buffer >> buffer >> num;
	for (int i = 0; i < num; ++i)
	{
		fin >> buffer;
		Shader->GetTexture2D()->Add(buffer);
	}


	// 샘플러
	fin >> buffer;
	fin >> buffer >> buffer >> num;
	int Filter; int adsU; int adsV; int adsW;
	float MipLODBias; int MasAnisotropy; int CompareFunc;
	float color1; float color2; float color3; float color4;
	float MinLod; float MaxLod;
	for (int i = 0; i < num; ++i)
	{
		fin >> Filter >> adsU >> adsV >> adsW >> MipLODBias >> MasAnisotropy
			>> CompareFunc >> color1 >> color2 >> color3 >> color4 >> MinLod >> MaxLod;
		D3D11_SAMPLER_DESC desc;
		desc.Filter = (D3D11_FILTER)Filter;
		desc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)adsU; 
		desc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)adsV;
		desc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)adsW;
		desc.MipLODBias = MipLODBias;
		desc.MaxAnisotropy = MasAnisotropy;
		desc.ComparisonFunc = (D3D11_COMPARISON_FUNC)CompareFunc;
		desc.BorderColor[0] = color1; 	desc.BorderColor[1] = color2;
		desc.BorderColor[2] = color3; 	desc.BorderColor[3] = color4;
		desc.MinLOD = MinLod; desc.MaxLOD = MaxLod;
		Shader->GetSamplerState()->AddCustomMode(desc);
	}
	if (!Shader->GetSamplerState()->CreateSamplerState())
	{
		JGLOG(log_Error, "JGRC::ShaderReader", "Failed Create SamplerState // " + Shader->GetPath());
	}

}