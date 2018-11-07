#include"MaterialReader.h"
#include"Material.h"
#include"Shader/ShaderTool/ShaderCompiler.h"
#include"Shader/ShaderTool/ShaderObjectManager.h"
#include"MaterialSystem.h"
#include"SceneRenderSystem/SRSMaterial.h"
using namespace JGRC;
using namespace std;
MaterialSystem* MaterialReader::m_MtSys = nullptr;
ShaderObjectManager* MaterialReader::m_ObjManager = nullptr;
MaterialReader::MaterialReader()
{
	m_ObjManager = ShaderObjectManager::GetInstance();
	m_MtSys = MaterialSystem::GetInstance();
}
MaterialReader::~MaterialReader()
{

}
Material* MaterialReader::Read(const string& materialPath)
{
	Material* mt = m_MtSys->CreateMaterial();

	ReadMaterialFile(materialPath, mt);

	return mt;
}
bool  MaterialReader::SRS_Read(const std::string& materialPath, SRSMaterial* mt)
{
	Material* mtial = (Material*)mt;
	ReadMaterialFile(materialPath, mtial);

	if (mtial)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void MaterialReader::ReadMaterialFile(const string& materialPath, Material* mt)
{
	fstream fin;
	fin.open(materialPath);
	string buffer;
	if (!fin.is_open())
	{
		JGLOG(log_Error, "JGRC::MaterialReader", materialPath + "Failed Read MaterialFile");
		mt = nullptr;
		return;
	}
	uint loopCount = 0;
	while (!fin.eof())
	{
		EShaderType type;
		string path;
		InputLayoutData lay;

		fin >> buffer;
		// 타입 저장
		if (ReadShaderType(buffer, fin, &type))
		{
			ShaderObject* obj = m_ObjManager->CreateShaderObject(type);
			mt->AddShaderObject(obj);
		}
		else if (ReadHlslPath(buffer, fin, &path))
		{
			mt->AddHlslPath(path);
		}
		else if (ReadInputLayout(buffer, fin, mt))
		{

		}
		else if (ReadCBuffer(buffer, fin, mt, type))
		{

		}
		else if (ReadSamplerState(buffer, fin, mt))
		{

		}
		else if (ReadTexture(buffer, fin, mt))
		{

		}

	}
	fin.close();
	if (!mt->ShaderCompile())
	{
		JGLOG(log_Error, "JGRC::MaterialReader", materialPath + "Failed MaterialShaderCompile");
		mt = nullptr;
		return;
	}
}
bool MaterialReader::ReadShaderType(const string& buffer, fstream& fin, EShaderType* type)
{
	if (!Move(buffer, "%ShaderType", fin))
	{
		return false;
	}

	int result;

	fin >> result;

	*type = (EShaderType)result;
	return true;
}
bool MaterialReader::ReadHlslPath(const string& buffer, fstream& fin, string* path)
{
	if (!Move(buffer, "%Hlsl", fin))
	{
		return false;
	}
	fin >> *path;

	return true;
}
bool MaterialReader::ReadInputLayout(const string& buffer, fstream& fin, Material* mt)
{
	 // @@InputLayout
	if (!Move(buffer, "@@InputLayout", fin))
	{
		return false;
	}
	uint count = 0;
	fin >> count;
	if (count == 0) return false;

	InputLayoutData data;
	D3D11_INPUT_ELEMENT_DESC desc;
	string SementicName;
	uint format;
	uint InputSlotClass;
	for (uint i = 0; i < count; ++i)
	{
		fin >> SementicName >> desc.SemanticIndex >> format >> desc.InputSlot
			>> desc.AlignedByteOffset >> InputSlotClass >> desc.InstanceDataStepRate;
		desc.Format = (DXGI_FORMAT)format;
		desc.InputSlotClass = (D3D11_INPUT_CLASSIFICATION)InputSlotClass;
		data.vSementicNames.push_back(move(SementicName));
		data.vDescs.push_back(move(desc));
	}
	uint stride = 1;
	while (true)
	{
		fin >> stride;
		if (stride == 0)
			break;
		data.vStrides.push_back(stride);
	}
	mt->SetInputLayout(data);
	return true;
}
bool MaterialReader::ReadCBuffer(const string & buffer, fstream & fin, class Material* mt, EShaderType type)
{
	if (!Move(buffer, "@@CBuffer",fin))
	{
		return false;
	}
	string tmp;
	uint Count = 0;
	fin >> Count;
	if (Count == 0) return false;
	for (uint i = 0; i < Count; ++i)
	{
		CBufferData data;
		uint varCount = 0;
		fin >> tmp >> tmp >> data.Name;
		fin >> tmp >> tmp >> data.Size;
		fin >> tmp >> tmp >> varCount;

		for (uint j = 0; j < varCount; ++j)
		{
			CBufferData::Var V;
			uint varCount = 0;
			fin >> tmp >> tmp >> V.Name >> varCount;
			V.varInit(varCount);

			data.vVars.push_back(V);
		}
		mt->AddCBuffer(data,type);
	}
	return true;
}
bool MaterialReader::ReadSamplerState(const string& buffer, fstream& fin, Material* mt)
{
	if (!Move(buffer, "@@SamplerState", fin))
	{
		return false;
	}
	uint Count = 0;
	fin >> Count;
	if (Count == 0) return false;
	SamplerStateData data;

	string tmp;
	int mode = 0;
	int nfilter = 0;
	int nAu = 0;
	int nAv = 0;
	int nAw = 0;
	for (uint i = 0; i < Count; ++i)
	{
		D3D11_SAMPLER_DESC desc;
		fin >> tmp >> tmp >> mode;
		data.vModes.push_back((ESamplerMode)mode);

		fin >> nfilter >> nAu >> nAv >> nAw >> desc.MipLODBias >> desc.MaxAnisotropy
			>> desc.BorderColor[0] >> desc.BorderColor[1] >> desc.BorderColor[2] >> desc.BorderColor[3]
			>> desc.MinLOD >> desc.MaxLOD;
		desc.Filter = (D3D11_FILTER)nfilter;
		desc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)nAu;
		desc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)nAv;
		desc.AddressW = (D3D11_TEXTURE_ADDRESS_MODE)nAw;

		data.vDescs.push_back(desc);
	}
	mt->SetSamplerState(data);
	return true;
}
bool MaterialReader::ReadTexture(const string & buffer, fstream & fin, Material* mt)
{
	if (!Move(buffer, "@@Texture", fin))
	{
		return false;
	}
	uint Count = 0;
	fin >> Count;
	if (Count == 0) return false;
	TextureData data;
	for (uint i = 0; i < Count; ++i)
	{
		string name;
		int n;
		fin >> n >> name;
		data.vNames.push_back(move(name));
	}
	mt->SetTexture(data);
	return true;
}
bool MaterialReader::Move(const string& buffer, const string& keyword, fstream& fin)
{
	if (!StringUtil::FindString(buffer, keyword.c_str()))
	{
		return false;
	}
	string tmp;
	while (tmp != ":")
	{
		fin >> tmp;
	}
	return true;
}
